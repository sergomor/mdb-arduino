/*
  LlistT.h

  Copyright (c) 2016, Sergey Morgalev sergomor@gmail.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/
#ifndef LListT_h
#define LListT_h

#include <inttypes.h>

/*

Simple Linked List

*/

template <typename T>
class LlistT
{
public:
	struct Node
	{
		T data;
		Node* next = nullptr;
		Node(const T& _data): data(_data){}
		~Node(){ delete next; }
	};

	struct Iterator
	{
		Node* node;
		Iterator(Node* n):node(n){}
		T& operator *(){ return node->data; }
		bool operator ==(const Iterator& i){ return node == i.node; }
		bool operator !=(const Iterator& i){ return node != i.node; }
	};

	LlistT(){};
	~LlistT(){ delete first; };
	uint16_t itemCount(){ return count; }
	uint16_t add(const T&);
	void reuse(){ count = 0; current = last = first;}
	Iterator begin()	{ return Iterator(0 == count ? nullptr : current = first); }
	Iterator next()	{ return Iterator(current = current->next); }
	Iterator end()		{ return Iterator(0 == count ? nullptr : last->next); }

	//T* lastItem(){ return &(last->data); }
 
private:
	Node* first = nullptr;
	Node* current = nullptr;
	Node* last = nullptr;

	uint16_t size = 0;
	uint16_t count = 0;
};

template <typename T>
uint16_t LlistT<T>::add(const T& item)
{
	if (first == nullptr)
	{
		// init list with first node
		last = current = first = new Node(item);
		return count = size = 1;
	}
	else
	{
		++count;
		if (size >= count)
		{
			// reuse old node
			current = first;
			uint16_t index = 0;
			while (++index < count) current = current->next;
			current->data = item;
			last = current;
		}
		else
		{
			// create new node
			last = last->next = new Node(item);
			size = count;
		}
	}
	return count; 
}

#endif