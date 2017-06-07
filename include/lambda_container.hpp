#pragma once

#include "heap.hpp"
#include "hanastd.hpp"

//http://hczhcz.github.io/2014/11/04/type-and-storage-of-lambda.html
class LambdaContainer {
private:
    void *buf;
	int size;

    template <class T>
    struct Helper {
        static void call(void *ptr) {
            return ((T *) ptr)->operator()();
        }

        static void del(void *ptr) {
            //delete (T *) ptr;
        }
    };

    void (*call)(void *ptr);
    void (*del)(void *ptr);

public:
    template <class T>
    LambdaContainer(T &data) {
		buf = (void *) malloc(sizeof(T));
		size=sizeof(T);
 		hanastd::strncpy((char*)&data,(char*)buf,size);
        call = Helper<T>::call;
        del = Helper<T>::del;
    }

    template <class T>
    LambdaContainer(T &&data) {
		buf = (void *) malloc(sizeof(T));
		size=sizeof(T);
 		hanastd::strncpy((char*)&data,(char*)buf,size);
        call = Helper<T>::call;
        del = Helper<T>::del;
    }

    template <class T>
    void init(T &&data) {
        buf = (void *) data;
    }

    template <class T>
    void init(T &data) {
        buf = (void *) data;
    }

    void operator()() {
        return call(buf);
    }

    ~LambdaContainer() {
        //mfree((uintptr_t)buf,size);
    }

    void destroy() {
        mfree((uintptr_t)buf,size);
    }
};
