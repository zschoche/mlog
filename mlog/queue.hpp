#ifndef ___MLOG_QUEUE_HPP___
#define ___MLOG_QUEUE_HPP___

#include <atomic>

namespace mlog {

template <typename T> class queue {
      private:
	struct node {
		node() : next(nullptr) {}
		node(T val) : value(std::move(val)), next(nullptr) {}
		T value;
		node *next;
	};

	node *first;			   
	std::atomic<node *> divider, last; 

      public:
	queue() {
		first = divider = last = new node(); 
	}
	~queue() {
		while (first != nullptr) { 
			node *tmp = first;
			first = tmp->next;
			delete tmp;
		}
	}


	//http://preshing.com/20130922/acquire-and-release-fences/
	//
	template <typename S> void push(S &&item) {
		static_cast<node *>(last)->next =
		    new node(std::forward<S>(item));
		last.store(static_cast<node *>(last)->next, std::memory_order_relaxed);
		while (first != divider) {
			node *tmp = first;
			first = first->next;
			delete tmp;
		}
	}

	bool pop(T &result) {
		if (divider != last.load(std::memory_order_acquire)) {
			result = static_cast<node *>(divider)->next->value;
			divider = static_cast<node *>(divider)->next;
			return true;
		}
		return false;
	}
};

} /*  mlog */

#endif
