//
// Created by linhdh on 22/09/2025.
//
#include <czmq.h>
#define WORKER_READY   "\001"      //  Signals worker is ready

int main ()
{
    zsock_t *frontend = zsock_new (ZMQ_ROUTER);
    zsock_t *backend = zsock_new (ZMQ_ROUTER);
    zsock_bind (frontend, "tcp://*:55550");    //  For clients
    zsock_bind (backend,  "tcp://*:55560");    //  For workers

    //  Queue of available workers
    zlist_t *workers = zlist_new ();

    //  The body of this example is exactly the same as lbbroker2.
    //  .skip
    zmq_pollitem_t pollitems [] = {
        { backend,  0, ZMQ_POLLIN, 0 },
        { frontend, 0, ZMQ_POLLIN, 0 }
    };

    while (true) {
        //  Poll frontend only if we have available workers
        if (const int rc = zmq_poll (pollitems, zlist_size (workers) > 0 ? 2 : 1, -1); rc == -1)
            break;              //  Interrupted

        //  Handle worker activity on backend
        if (pollitems [0].revents & ZMQ_POLLIN) {
            //  Use worker identity for load-balancing
            zmsg_t *msg = zmsg_recv (backend);
            if (!msg)
                break;          //  Interrupted
            zframe_t *identity = zmsg_unwrap (msg);
            zlist_append (workers, identity);

            //  Forward message to client if it's not a READY
            zframe_t *frame = zmsg_first (msg);
            if (memcmp (zframe_data (frame), WORKER_READY, 1) == 0)
                zmsg_destroy (&msg);
            else
                zmsg_send (&msg, frontend);
        }
        if (pollitems [1].revents & ZMQ_POLLIN) {
            //  Get client request, route to first available worker
            zmsg_t *msg = zmsg_recv (frontend);
            if (msg) {
                zmsg_wrap (msg, static_cast<zframe_t *>(zlist_pop(workers)));
                zmsg_send (&msg, backend);
            }
        }
    }
    //  When we're done, clean up properly
    while (zlist_size (workers)) {
        auto *frame = static_cast<zframe_t *>(zlist_pop(workers));
        zframe_destroy (&frame);
    }
    zlist_destroy (&workers);
    zsock_destroy(&frontend);
    zsock_destroy(&backend);
    return 0;
    //  .until
}

