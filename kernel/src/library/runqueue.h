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

template<typename T> class RunQueueNode
{
public:

	T Data;

	RunQueueNode<T> *Previous;
	RunQueueNode<T> *Next;

	RunQueueNode()
	{
		Previous = nullptr;
		Next = nullptr;
	}

	RunQueueNode(T DataInitial)
	{
		Data = DataInitial;
		Previous = nullptr;
		Next = nullptr;
	}
};

template<typename T> class RunQueue
{
private:

	RunQueueNode<T> *Head;
	RunQueueNode<T> *Tail;
	RunQueueNode<T> *Active;

public:

	RunQueue()
	{
		Head = nullptr;
		Tail = nullptr;
		Active = nullptr;
	}

	~RunQueue()
	{
		//
	}

	RunQueueNode<T> *GetActive()
	{
		if(Active == nullptr) Active = Head;

		return Active;
	}

	void SwitchNext()
	{
		if(Active != nullptr) Active = Active->Next;

		if(Active == nullptr) Active = Head;
	}

	void SwitchByAlgo(UInt64 Algo)
	{
		//TODO: Switch by different Algos here

		if(Active != nullptr) Active = Active->Next;

		if(Active == nullptr) Active = Head;
	}

	RunQueueNode<T> *GetFirstElement()
	{
		return Head;
	}

	RunQueueNode<T> *GetLastElement()
	{
		return Tail;
	}

	RunQueueNode<T> *GetElementAt(int Position)
	{
		RunQueueNode<T> *Current = Head;
		for(int i = 0; i < Position && Current != nullptr; i++, Current = Current->Next);
		return Current;
	}

	void AddElementBetween(T Data, RunQueueNode<T> *Previous, RunQueueNode<T> *Next)
	{
		//Data
		RunQueueNode<T> *New = new RunQueueNode<T>();
		New->Data = Data;

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
		RunQueueNode<T> *Current = GetElementAt(n - 1);

		//Surrounding
		RunQueueNode<T> *Previous = nullptr;
		RunQueueNode<T> *Next = nullptr;

		//Found Position in RunQueue -> Between Previous + Found
		if(Current != nullptr)
		{
			Previous = Current->Previous;
			Next = Current;
		}
		//Position Exceeds RunQueue -> Between Tail + Null (if found at Position 0, Found is Null, but Head also, so is Tail, so no special Handling for Found == Head)
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
		RunQueueNode<T> *New = new RunQueueNode<T>();
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
		RunQueueNode<T> *New = new RunQueueNode<T>();
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

	T RemoveElement(RunQueueNode<T> *Current)
	{
		if(Current != nullptr)
		{
			RunQueueNode<T> *Previous = Current->Previous;
			RunQueueNode<T> *Next = Current->Next;

			if(Previous == nullptr) Head = Next;
			else Previous->Next = Next;

			if(Next == nullptr) Tail = Previous;
			else Next->Previous = Previous;

			if(Active = Current) Active = Next;

			T Data = Current->Data;
			FreeMemory(Current);

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
		RunQueueNode<T> *Current = GetElementAt(n);
		RemoveElement(Current);
	}

	T RemoveHead()
	{
		return RemoveElement(Head);

		/*if(Head != nullptr)
		{
			T Data = Head->Data;

			RunQueueNode<T> *First = Head;
			RunQueueNode<T> *Second = Head->Next;

			Head = Second;
			if(Second != nullptr) Second->Previous = nullptr;

			if(Second == nullptr) Tail = Head;

			FreeMemory(First);

			return Data;
		}*/
	}

	T RemoveTail()
	{
		return RemoveElement(Tail);

		/*if(Tail != nullptr)
		{
			T Data = Tail->Data;

			RunQueueNode<T> *Last = Tail;
			RunQueueNode<T> *Prelast = Tail->Previous;

			Tail = Prelast;
			if(Prelast != nullptr) Prelast->Next = nullptr;

			if(Prelast == nullptr) Head = Tail;

			FreeMemory(Last);

			return Data;
		}*/
	}

	struct Iterator
	{
		friend class RunQueue;

	private:

		RunQueueNode<T> *Current;

	public:

		Iterator(RunQueueNode<T> *Pointer) : Current(Pointer)
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
			RunQueueNode<T> *Previous = Current;
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
			RunQueueNode<T> *Next = Current;
			Current = Current->Previous;
			return Iterator(Next);
		}

		//*x
		const T& operator*() const
		{
			return Current->Data;
		}

		//=
		Iterator& operator=(RunQueueNode<T> *New)
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
