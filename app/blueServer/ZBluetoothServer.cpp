#include "ZBluetoothServer.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/hidp.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/poll.h>
#include <errno.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/socket.h>

#include <thread>


#include <dbus/dbus.h>


namespace A7 {


static float tv2fl(struct timeval tv)
{
  return (float)(tv.tv_sec*1000.0) + (float)(tv.tv_usec/1000.0);
}

void ZBluetoothServer::runService(TProtocol protocol,const char* mac, TCallback* hander)
{
  switch(protocol)
  {
    case SERVER_SPP:
      {
        TDevice* pdevice = new TDevice;
        pdevice->handler = hander;
        pdevice->protocol = protocol;
        strncpy(pdevice->Mac, mac, 17);
        pdevice->ServerThread = new std::thread(rfcomm_server_thread, pdevice);
        pdevice->ServerThread->detach();

        inserDev(pdevice);
      }
      break;
    case SERVER_HID:
      {
        TDevice* pdevice = new TDevice;
        hid_service(pdevice);
      }
      break;
    default:
      break;
  }
}

ZBluetoothServer::ZBluetoothServer()
{
  _bt_dev_id = hci_get_route(NULL);
  if(_bt_dev_id < 0)
  {
    return;
  }

  _btsock = hci_open_dev(_bt_dev_id);
  if(_btsock < 0)
  {
    return;
  }
}

ZBluetoothServer::~ZBluetoothServer()
{

}

void ZBluetoothServer::scan(std::map<std::string,std::string> &Devs)
{
    Devs.clear();
    inquiry_info * ii = new inquiry_info[255];
    if(_bt_dev_id >= 0)
    {
        int num_rsp = hci_inquiry(_bt_dev_id, 8, 255, NULL,&ii ,IREQ_CACHE_FLUSH);
        char addr[19];
        char name[100];
        for(int i = 0; i < num_rsp; i++)
        {
            memset(addr, 0, sizeof(addr));
            memset(name, 0, sizeof(name));
            ba2str(&ii[i].bdaddr, addr);
            hci_read_remote_name(_btsock, &ii[i].bdaddr,sizeof(name), name, 0);
            Devs.insert(std::pair<std::string,std::string> (addr, name) );
        }
    }
    delete[] ii;
}

void ZBluetoothServer::rfcomm_server_thread(TDevice* device)
{
  while(1)
  {
    int status;
    struct sockaddr_rc addr={0};
    int fb;
    if((fb = socket(PF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM)) < 0)
    {
      std::cout<<"create socket error!!\n"<<std::endl;
      sleep(1);
      continue;
    }

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)1;
    str2ba(device->Mac, &addr.rc_bdaddr);

    char buff[512];

    if( (status = connect(fb,(struct sockaddr *)&addr,sizeof(addr))) < 0)
    {
      std::cout<<"error connect"<<std::endl;
      close(fb);
      sleep(1);
      continue;
    }

    int flags = fcntl(fb, F_GETFL,0);
    fcntl(fb, F_SETFL, flags | O_NONBLOCK);

    struct timeval timeout;
    fd_set readfd;
    //创建ping线程，检测连接状态
    device->isPingOk = true;
    std::thread pingThread(L2ping_thread, device);
#if 1
    std::cout<<"start select"<<std::endl;
    for(;;)
    {
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        FD_ZERO(&readfd);
        FD_SET(fb, &readfd);
        int ret;

        if((ret = select(fb+1, &readfd, NULL,NULL,&timeout)) < 0)
        {
          std::cout<<"select error"<<std::endl;
          break;
        }

        if(!ret){
         // std::cout<<"timeout"<<std::endl;
          if(!device->isPingOk)
            break;
          else
            continue;
        }

        if(FD_ISSET(fb, &readfd))
        {
            memset(buff, 0, sizeof(buff));
            if((ret = read(fb, buff, sizeof(buff))) < 0)
            {
                std::cout<<"error Status:"<<ret<<"|"<<"errorno["<<errno<<"]"<<std::endl;
                sleep(1);
                break;
            }

            if(!ret)
            {
                std::cout<<"error Status:"<<ret<<"|"<<"errorno["<<errno<<"]"<<std::endl;
                continue;
            }

            if(buff[0] != '\0')
            {
              if(device->handler != NULL)
                device->handler(buff, ret);
              else
                std::cout<<"["<<ret<<"]-["<<buff<<"]"<<std::endl;
            }else{
              std::cout<<"an recv error, ignore \n";
            }
        }else{
            std::cout<<"other error:"<<std::endl;
           break;
        }
    } //for(;;)


#else

    std::cout<<"start poll"<<std::endl;

    struct pollfd pf[1];
    pf[1].fd = fb;

    for(;;)
    {
        pf[1].fd = fb;
        pf[1].events = POLLIN | POLLERR | POLLREMOVE;
        // pf[1].events = POLLIN ;
        int ret;
        if((ret = poll(pf, 1, 1000)) < 0)
        {
            std::cout<<"poll error"<<std::endl;
            break;
        }

        if(!ret)
        {
            std::cout<<"time out"<<std::endl;
            continue;
        }

        memset(buff, 0, sizeof(buff));
        if((ret = recv(fb, buff, sizeof(buff), O_NONBLOCK)) < 0)
        {
            std::cout<<"recv error"<<std::endl;
            break;
        }

        if(!ret) //ret==0
        {
            std::cout<<"Disconnected\n"<<std::endl;
            break;
        }

        std::cout<<"("<<ret<<")["<<buff<<"]"<<std::endl;

    }
#endif
    //等待ping线程结束
    pingThread.join();
    close(fb);
  }
}

void ZBluetoothServer::L2ping_thread(ZBluetoothServer::TDevice *device)
{
	struct sockaddr_l2 addr;
	socklen_t optlen;
	unsigned char *send_buf;
	unsigned char *recv_buf;
	char str[18];
	int i, sk, lost;
	uint8_t id;

  int size    = 44;
  int ident   = 200;
  bdaddr_t bdaddr = {0, 0, 0, 0, 0, 0};
  int count   = -1;
  /* Defaults */
  int reverse = 0;
  int verify = 0;

  /* Stats */
  int sent_pkt = 0;
  int recv_pkt = 0;

  send_buf =(unsigned char*) malloc(L2CAP_CMD_HDR_SIZE + size);
  if(!send_buf)
  {
      perror("Can't allocate buffer");
      return;
  }

  recv_buf =(unsigned char*) malloc(L2CAP_CMD_HDR_SIZE + size);
  if (!recv_buf)
  {
      perror("Can't allocate buffer");
      free(send_buf);
      return;
  }


  /* Create socket */
  sk = socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_L2CAP);
  if (sk < 0) {
      perror("Can't create socket");
      goto ping_error;
  }

  /* Bind to local address */
  memset(&addr, 0, sizeof(addr));
  addr.l2_family = AF_BLUETOOTH;
  bacpy(&addr.l2_bdaddr, &bdaddr);
  if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
      perror("Can't bind socket");
      goto ping_error;
  }

  /* Connect to remote device */
  memset(&addr, 0, sizeof(addr));
  addr.l2_family = AF_BLUETOOTH;
  str2ba(device->Mac, &addr.l2_bdaddr);

  if (connect(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
      perror("Can't connect");
      goto ping_error;
  }

  /* Get local address */
  memset(&addr, 0, sizeof(addr));
  optlen = sizeof(addr);

  if (getsockname(sk, (struct sockaddr *) &addr, &optlen) < 0) {
      perror("Can't get local address");
      goto ping_error;
  }

  ba2str(&addr.l2_bdaddr, str);

  /* Initialize send buffer */
  for (i = 0; i < size; i++)
      send_buf[L2CAP_CMD_HDR_SIZE + i] = (i % 40) + 'A';

  id = ident;

  while (count == -1 || count-- > 0)
  {
      struct timeval tv_send, tv_recv, tv_diff;
      l2cap_cmd_hdr *send_cmd = (l2cap_cmd_hdr *) send_buf;
      l2cap_cmd_hdr *recv_cmd = (l2cap_cmd_hdr *) recv_buf;

      /* Build command header */
      send_cmd->ident = id;
      send_cmd->len   = htobs(size);

      if (reverse)
          send_cmd->code = L2CAP_ECHO_RSP;
      else
          send_cmd->code = L2CAP_ECHO_REQ;

      gettimeofday(&tv_send, NULL);

      /* Send Echo Command */
      if (send(sk, send_buf, L2CAP_CMD_HDR_SIZE + size, 0) <= 0) {
          perror("Send failed");
          goto ping_error;
      }

      /* Wait for Echo Response */
      lost = 0;
      while (1) {
          struct pollfd pf[1];
          int err;

          pf[0].fd = sk;
          pf[0].events = POLLIN;

          if ((err = poll(pf, 1, 1 * 1000)) < 0) {
              perror("Poll failed");
              goto ping_error;
          }

          if (!err) {
              lost = 1;
              break;
          }

          if ((err = recv(sk, recv_buf, L2CAP_CMD_HDR_SIZE + size, 0)) < 0) {
              perror("Recv failed");
              goto ping_error;
          }

          if (!err){
              printf("Disconnected\n");
              goto ping_error;
          }

          recv_cmd->len = btohs(recv_cmd->len);

          /* Check for our id */
          if (recv_cmd->ident != id)
              continue;

          /* Check type */
          if (!reverse && recv_cmd->code == L2CAP_ECHO_RSP)
              break;

          if (recv_cmd->code == L2CAP_COMMAND_REJ) {
              std::cout<<"Peer doesn't support Echo packets"<<std::endl;

              close(sk);
              free(send_buf);
              free(recv_buf);
              return;
          }

      }
      sent_pkt++;

      if (!lost) {
          recv_pkt++;

          gettimeofday(&tv_recv, NULL);
          timersub(&tv_recv, &tv_send, &tv_diff);

          if (verify) {
              /* Check payload length */
              if (recv_cmd->len != size) {
                  fprintf(stderr, "Received %d bytes, expected %d\n",
                          recv_cmd->len, size);
                  goto ping_error;
              }

              /* Check payload */
              if (memcmp(&send_buf[L2CAP_CMD_HDR_SIZE],
                         &recv_buf[L2CAP_CMD_HDR_SIZE], size)) {
                  fprintf(stderr, "Response payload different.\n");
                  goto ping_error;
              }
          }

          if(0) {
              char logbuff[512];
              sprintf(logbuff, "%d bytes from %s id %d time %.2fms", recv_cmd->len, device->Mac,
                      id - ident, tv2fl(tv_diff));
              std::cout<<logbuff<<std::endl;
          }

          sleep(1);
      } else {
          printf("no response from %s: id %d\n", device->Mac, id - ident);
          goto ping_error;
      }

      if (++id > 254)
          id = ident;
  }

  close(sk);
  free(send_buf);
  free(recv_buf);
  device->isPingOk = false;
  return;
ping_error:
  close(sk);
  free(send_buf);
  free(recv_buf);
  device->isPingOk = false;
}


static int l2cap_listen(const bdaddr_t *bdaddr, unsigned short psm, int lm, int backlog)
{
	struct sockaddr_l2 addr;
	struct l2cap_options opts;
	int sk;

	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, bdaddr);
	addr.l2_psm = htobs(psm);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(sk);
		return -1;
	}

	setsockopt(sk, SOL_L2CAP, L2CAP_LM, &lm, sizeof(lm));

	memset(&opts, 0, sizeof(opts));
	opts.imtu = HIDP_DEFAULT_MTU;
	opts.omtu = HIDP_DEFAULT_MTU;
	opts.flush_to = 0xffff;

	setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts));

	if (listen(sk, backlog) < 0) {
		close(sk);
		return -1;
	}

	return sk;
}


static int l2cap_accept(int sk, bdaddr_t *bdaddr)
{
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	int nsk;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if ((nsk = accept(sk, (struct sockaddr *) &addr, &addrlen)) < 0)
		return -1;

	if (bdaddr)
		bacpy(bdaddr, &addr.l2_bdaddr);

	return nsk;
}


void ZBluetoothServer::hid_server(ZBluetoothServer::TDevice *device)
{
  std::cout<<"sdfas"<<std::endl;
  int fdctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HIDP);
  if(fdctl < 0)
  {
    std::cout<<"Can't open HIDP Control Socket"<<std::endl;
    return;
  }

  int csk, isk;
  bdaddr_t bdaddr = {0, 0, 0, 0, 0, 0};

  csk = l2cap_listen(&bdaddr,0x11, 0,10);
  if(csk < 0)
  {
    std::cout<<"cant't listen on HID Control channel"<<std::endl;
    return;
  }
  std::cout<<"listen control over"<<std::endl;

  isk = l2cap_listen(&bdaddr, 0x13, 0, 10);
  if( isk < 0)
  {
    std::cout<<"can't listen on hid interrupt"<<std::endl;
    return;
  }
  std::cout<<"listen interrupt over"<<std::endl;





  struct pollfd p[2];
  p[0].fd = csk;
  p[0].events = POLLIN | POLLERR | POLLHUP;

  p[1].fd = csk;
  p[1].events = POLLIN | POLLERR | POLLHUP;

	sigset_t sigs;
	short events;
	int err, ncsk, nisk;
	sigfillset(&sigs);
	sigdelset(&sigs, SIGCHLD);
	sigdelset(&sigs, SIGPIPE);
	sigdelset(&sigs, SIGTERM);
	sigdelset(&sigs, SIGINT);
	sigdelset(&sigs, SIGHUP);

  while(1)
  {
    p[0].revents = 0;
    p[1].revents = 0;
    if (ppoll(p,2 , NULL, &sigs) < 1)
      continue;

    events = p[0].revents | p[1].revents;
    if(events & POLLIN){
      std::cout<<"event pollin"<<std::endl;
      ncsk = l2cap_accept(csk, NULL);
      nisk = l2cap_accept(isk, NULL);


    }else if(events & POLLHUP){
      std::cout<<"event pollhup"<<std::endl;
    }else if(events & POLLERR){
      std::cout<<"event pollErr"<<std::endl;
    }else{
      std::cout<<"no event"<<std::endl;
    }

  }


}

static char *get_default_adapter_path(DBusConnection *conn)
{
	DBusMessage *msg, *reply;
	DBusError err;
	const char *reply_path;
	char *path;

	msg = dbus_message_new_method_call("org.bluez", "/",
					"org.bluez.Manager", "DefaultAdapter");

	if (!msg) {
		fprintf(stderr, "Can't allocate new method call\n");
		return NULL;
	}

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		fprintf(stderr,
			"Can't get default adapter\n");
		if (dbus_error_is_set(&err)) {
			fprintf(stderr, "%s\n", err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if (!dbus_message_get_args(reply, &err,
					DBUS_TYPE_OBJECT_PATH, &reply_path,
					DBUS_TYPE_INVALID)) {
		fprintf(stderr,
			"Can't get reply arguments\n");
		if (dbus_error_is_set(&err)) {
			fprintf(stderr, "%s\n", err.message);
			dbus_error_free(&err);
		}
		dbus_message_unref(reply);
		return NULL;
	}

	path = strdup(reply_path);

	dbus_message_unref(reply);

	dbus_connection_flush(conn);

	return path;
}


static char *get_adapter_path(DBusConnection *conn, const char *adapter)
{
	DBusMessage *msg, *reply;
	DBusError err;
	const char *reply_path;
	char *path;

	if (!adapter)
		return get_default_adapter_path(conn);

	msg = dbus_message_new_method_call("org.bluez", "/",
					"org.bluez.Manager", "FindAdapter");

	if (!msg) {
		fprintf(stderr, "Can't allocate new method call\n");
		return NULL;
	}

	dbus_message_append_args(msg, DBUS_TYPE_STRING, &adapter,
					DBUS_TYPE_INVALID);

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		fprintf(stderr,
			"Can't find adapter %s\n", adapter);
		if (dbus_error_is_set(&err)) {
			fprintf(stderr, "%s\n", err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if (!dbus_message_get_args(reply, &err,
					DBUS_TYPE_OBJECT_PATH, &reply_path,
					DBUS_TYPE_INVALID)) {
		fprintf(stderr,
			"Can't get reply arguments\n");
		if (dbus_error_is_set(&err)) {
			fprintf(stderr, "%s\n", err.message);
			dbus_error_free(&err);
		}
		dbus_message_unref(reply);
		return NULL;
	}

	path = strdup(reply_path);

	dbus_message_unref(reply);

	dbus_connection_flush(conn);

	return path;
}



void ZBluetoothServer::hid_service(ZBluetoothServer::TDevice *device)
{
  DBusConnection *conn;
  conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
  if(!conn)
  {
    std::cout<<"can't dbus bus!"<<std::endl;
    return;
  }






}

void ZBluetoothServer::inserDev(ZBluetoothServer::TDevice* &device)
{
  _DevsMutex.lock();
  _Devs.push_back(device);
  _DevsMutex.unlock();
}

void ZBluetoothServer::popDev(ZBluetoothServer::TDevice* &device, const char* mac)
{
  _DevsMutex.lock();
  for(auto iter=_Devs.begin(); iter != _Devs.end();)
  {
    std::string cmac = (*iter)->Mac;
    std::string wmac = mac;
    if(cmac== wmac)
    {
      device = (*iter);
      iter = _Devs.erase(iter);
      break;
    }
  }
  _DevsMutex.unlock();
}

void ZBluetoothServer::listDev(std::list<ZBluetoothServer::TDevice *> &Devs)
{
  _DevsMutex.lock();
  for(auto &pdev: _Devs)
  {
    Devs.push_back(pdev);
  }

  _DevsMutex.unlock();
}


}
