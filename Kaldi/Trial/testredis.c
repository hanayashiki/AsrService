#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>

void onMessage(redisAsyncContext *c, void* reply, void *privdata) {
	std::cout << "got message" << std::endl;
	redisReply *r = (redisReply*)reply;
	if (reply == NULL) return;

	printf("reply type: %d\n", r->type);	
	if (r->type == REDIS_REPLY_ARRAY && r->elements == 3) {
		if (strcmp(r->element[0]->str, "message") == 0) {
			printf("got message: %s\n", r->element[2]->str);		
		}
	}	
}

class Fuck {
public:
	static void fuck(redisAsyncContext * c, void * reply, void * privdata) {
		std::cout << "this is static void" << std::endl;
		std::cout << "pingSuccess called" << std::endl;
	}
};

int main() {
	redisContext *c;
	redisReply *reply;

	int port = 6379;

	struct timeval timeout = { 1, 500000 };
	c = redisConnectWithTimeout("127.0.0.1", port, timeout);
/*
	if (c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		} else {
			printf("Connection error: cannot alloc");
			return 1;
		}
	}

	reply = (redisReply*)redisCommand(c, "PING");
	printf("PING: %s\n", reply->str);
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(c, "DEL mylist");
	freeReplyObject(reply);

	for (int j = 0; j < 10; j++) {
		reply = (redisReply*)redisCommand(c, "LPUSH mylist element-%s",
			       std::to_string(j+1).c_str());
	}

	reply = (redisReply*)redisCommand(c, "LRANGE mylist 0 -1");
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (int j = 0; j < reply->elements; j++) {
			printf("%u)  %s\n", j, reply->element[j]->str);
		}
	}
	freeReplyObject(reply);
//	redisFree(c);

	reply = (redisReply*)redisCommand(c, "PUBLISH testtopic fuckyou");
	freeReplyObject(reply);

*/
	signal(SIGPIPE, SIG_IGN);
	struct event_base * base = event_base_new();

	printf("before redis async connect\n");
	redisAsyncContext * ac = redisAsyncConnect("127.0.0.1", 6379);
	printf("after redis async connect\n");

	if (ac->err) {
		printf("error: %s\n", c->errstr);
		return 1;
	}

	redisAsyncCommand(ac, Fuck::fuck, NULL, "PING");


	std::cout << "before redis libevent attach" << std::endl;
	redisLibeventAttach(ac, base);
	std::cout << "after redis libevent attach" << std::endl;
	redisAsyncCommand(ac, onMessage, NULL, "SUBSCRIBE testtopic");
	std::cout << "after subscribe" << std::endl;
	event_base_dispatch(base);
	std::cout << "after event_base_dispatch" << std::endl;
	
	redisFree(c);
	return 0;

}
