//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

template<typename T> class QueueNode
{
public:

	T Data;

	QueueNode<T> *Previous;
	QueueNode<T> *Next;

	QueueNode()
	{
		Previous = nullptr;
		Next = nullptr;
	}

	QueueNode(T DataInitial)
	{
		Data = DataInitial;
		Previous = nullptr;
		Next = nullptr;
	}
};

template<typename T> class Queue
{
private:

	QueueNode<T> *Head;
	QueueNode<T> *Tail;
	UInt64 Count;

public:

	Queue()
	{
		Head = nullptr;
		Tail = nullptr;
		Count = 0;
	}

	~Queue()
	{
		//
	}

	bool IsEmpty()
	{
		return (Head == nullptr);
	}

	UInt64 Size()
	{
		return Count;
	}

	void Enqueue(T Data)
	{
		//Data
		QueueNode<T> *New = new QueueNode<T>();
		New->Data = Data;

		//Last <> New
		if(Tail != nullptr) Tail->Next = New;
		New->Previous = Tail;

		//New <> Tail
		New->Next = nullptr;
		Tail = New;

		//Head <> New
		if(Head == nullptr) Head = New;

		//Count
		Count++;
	}

	T Dequeue()
	{
		if(Head != nullptr)
		{
			//Data
			T Data = Head->Data;

			//Surrounding
			QueueNode<T> *First = Head;
			QueueNode<T> *Second = Head->Next;

			//Head + Back
			Head = Second;
			if(Second != nullptr) Second->Previous = nullptr;

			//Tail
			if(Second == nullptr) Tail = Head;

			//Free
			FreeMemory(First);

			//Count
			Count--;

			//Result
			return Data;
		}
	}

	struct Iterator
	{
	private:

		QueueNode<T> *Current;

	public:

		Iterator(QueueNode<T> *Pointer) : Current(Pointer)
		{
			//
		}

		//++x
		Iterator& operator++()
		{
			if(Current != nullptr) Current = Current->Next;
			return *this;
		}

		//x++
		Iterator operator++(int)
		{
			QueueNode<T> *Previous = Current;
			Current = Current->Next;
			return Iterator(Previous);
		}

		//*x
		const T& operator*() const
		{
			return Current->Data;
		}

		//=
		Iterator& operator=(QueueNode<T> *New)
		{
			this->Current = New;
			return *this;
		}

		//==
		friend bool operator== (const Iterator& a, const Iterator& b)
		{
			return a.Current == b.Current;
		};

		//!=
		friend bool operator!= (const Iterator& a, const Iterator& b)
		{
			return a.Current != b.Current;
		};
	};

	Iterator begin()
	{
		return Iterator(Head);
	}

	Iterator end()
	{
		return Iterator(nullptr);
	}
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
