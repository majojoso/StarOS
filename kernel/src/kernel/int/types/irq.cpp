//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "irq.h"

#include<kernel/int/gen/registers.h>
#include<kernel/int/gen/idt.h>
#include<kernel/int/types/exceptions.h>
#include<kernel/int/pic/pic.h>
#include<kernel/int/apic/lapic.h>
#include<kernel/int/api/handler.h>

#include<kernel/ps/scheduler.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define HANDLER_IRQ(Id) extern "C" void HandlerIrq ## Id();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

HANDLER_IRQ(0)
HANDLER_IRQ(1)
HANDLER_IRQ(2)
HANDLER_IRQ(3)
HANDLER_IRQ(4)
HANDLER_IRQ(5)
HANDLER_IRQ(6)
HANDLER_IRQ(7)
HANDLER_IRQ(8)
HANDLER_IRQ(9)
HANDLER_IRQ(10)
HANDLER_IRQ(11)
HANDLER_IRQ(12)
HANDLER_IRQ(13)
HANDLER_IRQ(14)
HANDLER_IRQ(15)
#ifdef USE_APIC
HANDLER_IRQ(16)
HANDLER_IRQ(17)
HANDLER_IRQ(18)
HANDLER_IRQ(19)
HANDLER_IRQ(20)
HANDLER_IRQ(21)
HANDLER_IRQ(22)
HANDLER_IRQ(23)
HANDLER_IRQ(24)
HANDLER_IRQ(25)
HANDLER_IRQ(26)
HANDLER_IRQ(27)
HANDLER_IRQ(28)
HANDLER_IRQ(29)
HANDLER_IRQ(30)
HANDLER_IRQ(31)
HANDLER_IRQ(32)
HANDLER_IRQ(33)
HANDLER_IRQ(34)
HANDLER_IRQ(35)
HANDLER_IRQ(36)
HANDLER_IRQ(37)
HANDLER_IRQ(38)
HANDLER_IRQ(39)
HANDLER_IRQ(40)
HANDLER_IRQ(41)
HANDLER_IRQ(42)
HANDLER_IRQ(43)
HANDLER_IRQ(44)
HANDLER_IRQ(45)
HANDLER_IRQ(46)
HANDLER_IRQ(47)
HANDLER_IRQ(48)
HANDLER_IRQ(49)
HANDLER_IRQ(50)
HANDLER_IRQ(51)
HANDLER_IRQ(52)
HANDLER_IRQ(53)
HANDLER_IRQ(54)
HANDLER_IRQ(55)
HANDLER_IRQ(56)
HANDLER_IRQ(57)
HANDLER_IRQ(58)
HANDLER_IRQ(59)
HANDLER_IRQ(60)
HANDLER_IRQ(61)
HANDLER_IRQ(62)
HANDLER_IRQ(63)
HANDLER_IRQ(64)
HANDLER_IRQ(65)
HANDLER_IRQ(66)
HANDLER_IRQ(67)
HANDLER_IRQ(68)
HANDLER_IRQ(69)
HANDLER_IRQ(70)
HANDLER_IRQ(71)
HANDLER_IRQ(72)
HANDLER_IRQ(73)
HANDLER_IRQ(74)
HANDLER_IRQ(75)
HANDLER_IRQ(76)
HANDLER_IRQ(77)
HANDLER_IRQ(78)
HANDLER_IRQ(79)
HANDLER_IRQ(80)
HANDLER_IRQ(81)
HANDLER_IRQ(82)
HANDLER_IRQ(83)
HANDLER_IRQ(84)
HANDLER_IRQ(85)
HANDLER_IRQ(86)
HANDLER_IRQ(87)
HANDLER_IRQ(88)
HANDLER_IRQ(89)
HANDLER_IRQ(90)
HANDLER_IRQ(91)
HANDLER_IRQ(92)
HANDLER_IRQ(93)
HANDLER_IRQ(94)
HANDLER_IRQ(95)
HANDLER_IRQ(96)
HANDLER_IRQ(97)
HANDLER_IRQ(98)
HANDLER_IRQ(99)
HANDLER_IRQ(100)
HANDLER_IRQ(101)
HANDLER_IRQ(102)
HANDLER_IRQ(103)
HANDLER_IRQ(104)
HANDLER_IRQ(105)
HANDLER_IRQ(106)
HANDLER_IRQ(107)
HANDLER_IRQ(108)
HANDLER_IRQ(109)
HANDLER_IRQ(110)
HANDLER_IRQ(111)
HANDLER_IRQ(112)
HANDLER_IRQ(113)
HANDLER_IRQ(114)
HANDLER_IRQ(115)
HANDLER_IRQ(116)
HANDLER_IRQ(117)
HANDLER_IRQ(118)
HANDLER_IRQ(119)
HANDLER_IRQ(120)
HANDLER_IRQ(121)
HANDLER_IRQ(122)
HANDLER_IRQ(123)
HANDLER_IRQ(124)
HANDLER_IRQ(125)
HANDLER_IRQ(126)
HANDLER_IRQ(127)
HANDLER_IRQ(128)
HANDLER_IRQ(129)
HANDLER_IRQ(130)
HANDLER_IRQ(131)
HANDLER_IRQ(132)
HANDLER_IRQ(133)
HANDLER_IRQ(134)
HANDLER_IRQ(135)
HANDLER_IRQ(136)
HANDLER_IRQ(137)
HANDLER_IRQ(138)
HANDLER_IRQ(139)
HANDLER_IRQ(140)
HANDLER_IRQ(141)
HANDLER_IRQ(142)
HANDLER_IRQ(143)
HANDLER_IRQ(144)
HANDLER_IRQ(145)
HANDLER_IRQ(146)
HANDLER_IRQ(147)
HANDLER_IRQ(148)
HANDLER_IRQ(149)
HANDLER_IRQ(150)
HANDLER_IRQ(151)
HANDLER_IRQ(152)
HANDLER_IRQ(153)
HANDLER_IRQ(154)
HANDLER_IRQ(155)
HANDLER_IRQ(156)
HANDLER_IRQ(157)
HANDLER_IRQ(158)
HANDLER_IRQ(159)
HANDLER_IRQ(160)
HANDLER_IRQ(161)
HANDLER_IRQ(162)
HANDLER_IRQ(163)
HANDLER_IRQ(164)
HANDLER_IRQ(165)
HANDLER_IRQ(166)
HANDLER_IRQ(167)
HANDLER_IRQ(168)
HANDLER_IRQ(169)
HANDLER_IRQ(170)
HANDLER_IRQ(171)
HANDLER_IRQ(172)
HANDLER_IRQ(173)
HANDLER_IRQ(174)
HANDLER_IRQ(175)
HANDLER_IRQ(176)
HANDLER_IRQ(177)
HANDLER_IRQ(178)
HANDLER_IRQ(179)
HANDLER_IRQ(180)
HANDLER_IRQ(181)
HANDLER_IRQ(182)
HANDLER_IRQ(183)
HANDLER_IRQ(184)
HANDLER_IRQ(185)
HANDLER_IRQ(186)
HANDLER_IRQ(187)
HANDLER_IRQ(188)
HANDLER_IRQ(189)
HANDLER_IRQ(190)
HANDLER_IRQ(191)
HANDLER_IRQ(192)
HANDLER_IRQ(193)
HANDLER_IRQ(194)
HANDLER_IRQ(195)
HANDLER_IRQ(196)
HANDLER_IRQ(197)
HANDLER_IRQ(198)
HANDLER_IRQ(199)
HANDLER_IRQ(200)
HANDLER_IRQ(201)
HANDLER_IRQ(202)
HANDLER_IRQ(203)
HANDLER_IRQ(204)
HANDLER_IRQ(205)
HANDLER_IRQ(206)
HANDLER_IRQ(207)
HANDLER_IRQ(208)
HANDLER_IRQ(209)
HANDLER_IRQ(210)
HANDLER_IRQ(211)
HANDLER_IRQ(212)
HANDLER_IRQ(213)
HANDLER_IRQ(214)
HANDLER_IRQ(215)
HANDLER_IRQ(216)
HANDLER_IRQ(217)
HANDLER_IRQ(218)
HANDLER_IRQ(219)
HANDLER_IRQ(220)
//HANDLER_IRQ(221)
//HANDLER_IRQ(222)
//HANDLER_IRQ(223)
#endif

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler Spurious IRQ

bool HandleSpuriousIrq(int Id)
{
	//Detect Spurious IRQs
	UInt16 CurrentIsr = PicGetIsr();

	//Spurious IRQ7
	if(Id == 7 && (CurrentIsr & 0b0000000010000000) < 1)
	{
		//No EOI

		//Skip
		return true;
	}

	//Spurious IRQ15
	if(Id == 15 && (CurrentIsr & 0b1000000000000000) < 1)
	{
		//Master EOI (Slave had spurious interrupt [no EOI], Master sees a real one [EOI for Slave])
		PicIrqAck(7); //Done by faking IRQ7 to ACK function

		//Skip
		return true;
	}

	//Normal IRQ
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Handler PIC

extern "C" RegisterSet *HandlerIrqGeneralPic(int Id, RegisterSet *Registers)
{
	//Result
	RegisterSet *Result = Registers;

	//Handle Spurious Irq
	if(HandleSpuriousIrq(Id)) return Result;

	//Log
	#if 0
	if(Id != 0)
	{
		PrintFormatted("IRQ%d\r\n", Id);
		PrintFormatted("Registers: Code %d:%d Data %d:%d Flags %d\r\n", Registers->cs, Registers->ip, Registers->ss, Registers->sp, Registers->flags);
	}
	#endif

	//Handle
	if(Id == 0)
	{
		//Scheduler
		Result = SchedulerHandlerRoutine(Registers);

		//Handlers
		IrqHandlerRoutine(Registers, Id);
	}
	else
	{
		//Handlers
		IrqHandlerRoutine(Registers, Id);
	}

	//Acknowledge IRQ
	PicIrqAck(Id);

	//Result
	return Result;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Handler APIC

extern "C" RegisterSet *HandlerIrqGeneralApic(int Id, RegisterSet *Registers)
{
	//Result
	RegisterSet *Result = Registers;

	//Handle Spurious Irq
	//if(Id == 0 && HandleSpuriousIrq(Id)) return Result;
	//TODO: Replace 0 by correct vector for PIC and uncomment

	//Log
	#if 0
	if(Id != 0)
	{
		PrintFormatted("IRQ%d\r\n", Id);
		DumpRegisters(Registers);
	}
	#endif

	//Handle
	if(Id == 0)
	{
		//Scheduler
		Result = SchedulerHandlerRoutine(Registers);

		//Handlers
		IrqHandlerRoutine(Registers, Id);
	}
	else
	{
		//Handlers
		IrqHandlerRoutine(Registers, Id);
	}

	//Acknowledge IRQ
	LapicIrqAck();

	//Result
	return Result;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeIrq()
{
	//Fill IDT IRQs
	IdtSetEntryWithIstNum(EXCEPTIONS + 0,   (void *) HandlerIrq0,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 1,   (void *) HandlerIrq1,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 2,   (void *) HandlerIrq2,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 3,   (void *) HandlerIrq3,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 4,   (void *) HandlerIrq4,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 5,   (void *) HandlerIrq5,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 6,   (void *) HandlerIrq6,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 7,   (void *) HandlerIrq7,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 8,   (void *) HandlerIrq8,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 9,   (void *) HandlerIrq9,   ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 10,  (void *) HandlerIrq10,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 11,  (void *) HandlerIrq11,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 12,  (void *) HandlerIrq12,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 13,  (void *) HandlerIrq13,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 14,  (void *) HandlerIrq14,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 15,  (void *) HandlerIrq15,  ATTR_INTR_GATE, 2);
	#ifdef USE_APIC
	IdtSetEntryWithIstNum(EXCEPTIONS + 16,  (void *) HandlerIrq16,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 17,  (void *) HandlerIrq17,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 18,  (void *) HandlerIrq18,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 19,  (void *) HandlerIrq19,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 20,  (void *) HandlerIrq20,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 21,  (void *) HandlerIrq21,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 22,  (void *) HandlerIrq22,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 23,  (void *) HandlerIrq23,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 24,  (void *) HandlerIrq24,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 25,  (void *) HandlerIrq25,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 26,  (void *) HandlerIrq26,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 27,  (void *) HandlerIrq27,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 28,  (void *) HandlerIrq28,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 29,  (void *) HandlerIrq29,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 30,  (void *) HandlerIrq30,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 31,  (void *) HandlerIrq31,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 32,  (void *) HandlerIrq32,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 33,  (void *) HandlerIrq33,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 34,  (void *) HandlerIrq34,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 35,  (void *) HandlerIrq35,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 36,  (void *) HandlerIrq36,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 37,  (void *) HandlerIrq37,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 38,  (void *) HandlerIrq38,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 39,  (void *) HandlerIrq39,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 40,  (void *) HandlerIrq40,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 41,  (void *) HandlerIrq41,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 42,  (void *) HandlerIrq42,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 43,  (void *) HandlerIrq43,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 44,  (void *) HandlerIrq44,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 45,  (void *) HandlerIrq45,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 46,  (void *) HandlerIrq46,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 47,  (void *) HandlerIrq47,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 48,  (void *) HandlerIrq48,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 49,  (void *) HandlerIrq49,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 50,  (void *) HandlerIrq50,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 51,  (void *) HandlerIrq51,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 52,  (void *) HandlerIrq52,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 53,  (void *) HandlerIrq53,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 54,  (void *) HandlerIrq54,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 55,  (void *) HandlerIrq55,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 56,  (void *) HandlerIrq56,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 57,  (void *) HandlerIrq57,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 58,  (void *) HandlerIrq58,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 59,  (void *) HandlerIrq59,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 60,  (void *) HandlerIrq60,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 61,  (void *) HandlerIrq61,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 62,  (void *) HandlerIrq62,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 63,  (void *) HandlerIrq63,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 64,  (void *) HandlerIrq64,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 65,  (void *) HandlerIrq65,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 66,  (void *) HandlerIrq66,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 67,  (void *) HandlerIrq67,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 68,  (void *) HandlerIrq68,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 69,  (void *) HandlerIrq69,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 70,  (void *) HandlerIrq70,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 71,  (void *) HandlerIrq71,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 72,  (void *) HandlerIrq72,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 73,  (void *) HandlerIrq73,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 74,  (void *) HandlerIrq74,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 75,  (void *) HandlerIrq75,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 76,  (void *) HandlerIrq76,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 77,  (void *) HandlerIrq77,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 78,  (void *) HandlerIrq78,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 79,  (void *) HandlerIrq79,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 80,  (void *) HandlerIrq80,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 81,  (void *) HandlerIrq81,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 82,  (void *) HandlerIrq82,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 83,  (void *) HandlerIrq83,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 84,  (void *) HandlerIrq84,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 85,  (void *) HandlerIrq85,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 86,  (void *) HandlerIrq86,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 87,  (void *) HandlerIrq87,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 88,  (void *) HandlerIrq88,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 89,  (void *) HandlerIrq89,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 90,  (void *) HandlerIrq90,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 91,  (void *) HandlerIrq91,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 92,  (void *) HandlerIrq92,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 93,  (void *) HandlerIrq93,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 94,  (void *) HandlerIrq94,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 95,  (void *) HandlerIrq95,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 96,  (void *) HandlerIrq96,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 97,  (void *) HandlerIrq97,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 98,  (void *) HandlerIrq98,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 99,  (void *) HandlerIrq99,  ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 100, (void *) HandlerIrq100, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 101, (void *) HandlerIrq101, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 102, (void *) HandlerIrq102, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 103, (void *) HandlerIrq103, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 104, (void *) HandlerIrq104, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 105, (void *) HandlerIrq105, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 106, (void *) HandlerIrq106, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 107, (void *) HandlerIrq107, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 108, (void *) HandlerIrq108, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 109, (void *) HandlerIrq109, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 110, (void *) HandlerIrq110, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 111, (void *) HandlerIrq111, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 112, (void *) HandlerIrq112, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 113, (void *) HandlerIrq113, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 114, (void *) HandlerIrq114, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 115, (void *) HandlerIrq115, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 116, (void *) HandlerIrq116, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 117, (void *) HandlerIrq117, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 118, (void *) HandlerIrq118, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 119, (void *) HandlerIrq119, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 120, (void *) HandlerIrq120, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 121, (void *) HandlerIrq121, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 122, (void *) HandlerIrq122, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 123, (void *) HandlerIrq123, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 124, (void *) HandlerIrq124, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 125, (void *) HandlerIrq125, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 126, (void *) HandlerIrq126, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 127, (void *) HandlerIrq127, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 128, (void *) HandlerIrq128, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 129, (void *) HandlerIrq129, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 130, (void *) HandlerIrq130, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 131, (void *) HandlerIrq131, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 132, (void *) HandlerIrq132, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 133, (void *) HandlerIrq133, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 134, (void *) HandlerIrq134, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 135, (void *) HandlerIrq135, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 136, (void *) HandlerIrq136, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 137, (void *) HandlerIrq137, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 138, (void *) HandlerIrq138, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 139, (void *) HandlerIrq139, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 140, (void *) HandlerIrq140, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 141, (void *) HandlerIrq141, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 142, (void *) HandlerIrq142, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 143, (void *) HandlerIrq143, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 144, (void *) HandlerIrq144, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 145, (void *) HandlerIrq145, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 146, (void *) HandlerIrq146, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 147, (void *) HandlerIrq147, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 148, (void *) HandlerIrq148, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 149, (void *) HandlerIrq149, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 150, (void *) HandlerIrq150, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 151, (void *) HandlerIrq151, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 152, (void *) HandlerIrq152, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 153, (void *) HandlerIrq153, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 154, (void *) HandlerIrq154, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 155, (void *) HandlerIrq155, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 156, (void *) HandlerIrq156, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 157, (void *) HandlerIrq157, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 158, (void *) HandlerIrq158, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 159, (void *) HandlerIrq159, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 160, (void *) HandlerIrq160, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 161, (void *) HandlerIrq161, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 162, (void *) HandlerIrq162, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 163, (void *) HandlerIrq163, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 164, (void *) HandlerIrq164, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 165, (void *) HandlerIrq165, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 166, (void *) HandlerIrq166, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 167, (void *) HandlerIrq167, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 168, (void *) HandlerIrq168, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 169, (void *) HandlerIrq169, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 170, (void *) HandlerIrq170, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 171, (void *) HandlerIrq171, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 172, (void *) HandlerIrq172, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 173, (void *) HandlerIrq173, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 174, (void *) HandlerIrq174, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 175, (void *) HandlerIrq175, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 176, (void *) HandlerIrq176, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 177, (void *) HandlerIrq177, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 178, (void *) HandlerIrq178, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 179, (void *) HandlerIrq179, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 180, (void *) HandlerIrq180, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 181, (void *) HandlerIrq181, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 182, (void *) HandlerIrq182, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 183, (void *) HandlerIrq183, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 184, (void *) HandlerIrq184, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 185, (void *) HandlerIrq185, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 186, (void *) HandlerIrq186, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 187, (void *) HandlerIrq187, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 188, (void *) HandlerIrq188, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 189, (void *) HandlerIrq189, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 190, (void *) HandlerIrq190, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 191, (void *) HandlerIrq191, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 192, (void *) HandlerIrq192, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 193, (void *) HandlerIrq193, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 194, (void *) HandlerIrq194, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 195, (void *) HandlerIrq195, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 196, (void *) HandlerIrq196, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 197, (void *) HandlerIrq197, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 198, (void *) HandlerIrq198, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 199, (void *) HandlerIrq199, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 200, (void *) HandlerIrq200, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 201, (void *) HandlerIrq201, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 202, (void *) HandlerIrq202, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 203, (void *) HandlerIrq203, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 204, (void *) HandlerIrq204, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 205, (void *) HandlerIrq205, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 206, (void *) HandlerIrq206, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 207, (void *) HandlerIrq207, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 208, (void *) HandlerIrq208, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 209, (void *) HandlerIrq209, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 210, (void *) HandlerIrq210, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 211, (void *) HandlerIrq211, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 212, (void *) HandlerIrq212, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 213, (void *) HandlerIrq213, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 214, (void *) HandlerIrq214, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 215, (void *) HandlerIrq215, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 216, (void *) HandlerIrq216, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 217, (void *) HandlerIrq217, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 218, (void *) HandlerIrq218, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 219, (void *) HandlerIrq219, ATTR_INTR_GATE, 2);
	IdtSetEntryWithIstNum(EXCEPTIONS + 220, (void *) HandlerIrq220, ATTR_INTR_GATE, 2);
	//IdtSetEntryWithIstNum(EXCEPTIONS + 221, (void *) HandlerIrq221, ATTR_INTR_GATE, 2);
	//IdtSetEntryWithIstNum(EXCEPTIONS + 222, (void *) HandlerIrq222, ATTR_INTR_GATE, 2);
	//IdtSetEntryWithIstNum(EXCEPTIONS + 223, (void *) HandlerIrq223, ATTR_INTR_GATE, 2);
	#endif
}

void DeinitializeIrq()
{
	//
}
