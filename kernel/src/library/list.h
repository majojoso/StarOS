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

template<typename T> class ListNode
{
public:

	T Data;

	ListNode<T> *Previous;
	ListNode<T> *Next;

	ListNode()
	{
		Previous = nullptr;
		Next = nullptr;
	}

	ListNode(T DataInitial)
	{
		Data = DataInitial;
		Previous = nullptr;
		Next = nullptr;
	}
};

template<typename T> class List
{
private:

	ListNode<T> *Head;
	ListNode<T> *Tail;

public:

	List()
	{
		Head = nullptr;
		Tail = nullptr;
	}

	~List()
	{
		//
	}

	ListNode<T> *GetFirstElement()
	{
		return Head;
	}

	ListNode<T> *GetLastElement()
	{
		return Tail;
	}

	ListNode<T> *GetElementAt(int Position)
	{
		ListNode<T> *Current = Head;
		for(int i = 0; i < Position && Current != nullptr; i++, Current = Current->Next);
		return Current;
	}

	void AddElementBetween(T Data, ListNode<T> *Previous, ListNode<T> *Next)
	{
		//Data
		ListNode<T> *New = new ListNode<T>();
		New->Data = Data;
		New->Previous = nullptr;
		New->Next = nullptr;

		//Link Forward
		if(Previous == nullptr) Head = New;
		else Previous->Next = New;
		New->Next = Next;

		//Link Backward
		if(Next == nullptr) Tail = New;
		else Next->Previous = New;
		New->Previous = Previous;
	}

	void AddElementAt(T Data, int n)
	{
		//Find
		ListNode<T> *Current = GetElementAt(n - 1);

		//Surrounding
		ListNode<T> *Previous = nullptr;
		ListNode<T> *Next = nullptr;

		//Found Position in List -> Between Previous + Found
		if(Current != nullptr)
		{
			Previous = Current->Previous;
			Next = Current;
		}
		//Position Exceeds List -> Between Tail + Null (if found at Position 0, Found is Null, but Head also, so is Tail, so no special Handling for Found == Head)
		else
		{
			Previous = Tail;
			Next = nullptr;
		}

		//Add
		AddElementBetween(Data, Previous, Next);
	}

	void AddHead(T Data)
	{
		AddElementBetween(Data, nullptr, Head);

		/*//Data
		ListNode<T> *New = new ListNode<T>();
		New->Data = Data;

		//New <> First
		New->Next = Head;
		if(Head != nullptr) Head->Previous = New;

		//Head <> New
		Head = New;
		New->Previous = nullptr;

		//Tail <> New
		if(Tail == nullptr) Tail = New;*/
	}

	void AddTail(T Data)
	{
		AddElementBetween(Data, Tail, nullptr);

		/*//Data
		ListNode<T> *New = new ListNode<T>();
		New->Data = Data;

		//Last <> New
		if(Tail != nullptr) Tail->Next = New;
		New->Previous = Tail;

		//New <> Tail
		New->Next = nullptr;
		Tail = New;

		//Head <> New
		if(Head == nullptr) Head = New;*/
	}

	T RemoveElement(ListNode<T> *Current)
	{
		if(Current != nullptr)
		{
			ListNode<T> *Previous = Current->Previous;
			ListNode<T> *Next = Current->Next;

			if(Previous == nullptr) Head = Next;
			else Previous->Next = Next;

			if(Next == nullptr) Tail = Previous;
			else Next->Previous = Previous;

			T Data = Current->Data;
			delete Current;

			return Data;
		}
	}

	struct Iterator;
	T RemoveElement(Iterator CurrentIterator)
	{
		return RemoveElement(CurrentIterator.Current);
	}

	T RemoveElementAt(int n)
	{
		ListNode<T> *Current = GetElementAt(n);
		RemoveElement(Current);
	}

	T RemoveHead()
	{
		return RemoveElement(Head);

		/*if(Head != nullptr)
		{
			T Data = Head->Data;

			ListNode<T> *First = Head;
			ListNode<T> *Second = Head->Next;

			Head = Second;
			if(Second != nullptr) Second->Previous = nullptr;

			if(Second == nullptr) Tail = Head;

			delete First;

			return Data;
		}*/
	}

	T RemoveTail()
	{
		return RemoveElement(Tail);

		/*if(Tail != nullptr)
		{
			T Data = Tail->Data;

			ListNode<T> *Last = Tail;
			ListNode<T> *Prelast = Tail->Previous;

			Tail = Prelast;
			if(Prelast != nullptr) Prelast->Next = nullptr;

			if(Prelast == nullptr) Head = Tail;

			delete Last;

			return Data;
		}*/
	}

	struct Iterator
	{
		friend class List;

	private:

		ListNode<T> *Current;

	public:

		Iterator(ListNode<T> *Pointer) : Current(Pointer)
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
			ListNode<T> *Previous = Current;
			Current = Current->Next;
			return Iterator(Previous);
		}

		//--x
		Iterator& operator--()
		{
			if(Current != nullptr) Current = Current->Previous;
			return *this;
		}

		//x--
		Iterator operator--(int)
		{
			ListNode<T> *Next = Current;
			Current = Current->Previous;
			return Iterator(Next);
		}

		//*x
		const T& operator*() const
		{
			return Current->Data;
		}

		//=
		Iterator& operator=(ListNode<T> *New)
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

	Iterator rbegin()
	{
		return Iterator(Tail);
	}

	Iterator rend()
	{
		return Iterator(nullptr);
	}
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
