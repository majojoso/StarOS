;;============================================================================
;; Name        : StarOS
;; Description : x64 UEFI modern nonlegacy hybrid kernel operating system
;;============================================================================

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Includes

%include "kernel/int/types/regs.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Information

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definitions

%define USE_APIC 1

extern HandlerIrqGeneralPic
extern HandlerIrqGeneralApic

%macro Irq 1
	global HandlerIrq%1
	HandlerIrq%1:
		cli
		push 0
		PushRegisters
		mov rdi, %1
		mov rsi, rsp
		%ifdef USE_APIC
			call HandlerIrqGeneralApic
		%else
			call HandlerIrqGeneralPic
		%endif
		;mov rsp, rax
		PopRegisters
		add rsp, 8
		iretq
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Declarations

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Implementation

[bits 64]

Irq 0
Irq 1
Irq 2
Irq 3
Irq 4
Irq 5
Irq 6
Irq 7
Irq 8
Irq 9
Irq 10
Irq 11
Irq 12
Irq 13
Irq 14
Irq 15
%ifdef USE_APIC
Irq 16
Irq 17
Irq 18
Irq 19
Irq 20
Irq 21
Irq 22
Irq 23
Irq 24
Irq 25
Irq 26
Irq 27
Irq 28
Irq 29
Irq 30
Irq 31
Irq 32
Irq 33
Irq 34
Irq 35
Irq 36
Irq 37
Irq 38
Irq 39
Irq 40
Irq 41
Irq 42
Irq 43
Irq 44
Irq 45
Irq 46
Irq 47
Irq 48
Irq 49
Irq 50
Irq 51
Irq 52
Irq 53
Irq 54
Irq 55
Irq 56
Irq 57
Irq 58
Irq 59
Irq 60
Irq 61
Irq 62
Irq 63
Irq 64
Irq 65
Irq 66
Irq 67
Irq 68
Irq 69
Irq 70
Irq 71
Irq 72
Irq 73
Irq 74
Irq 75
Irq 76
Irq 77
Irq 78
Irq 79
Irq 80
Irq 81
Irq 82
Irq 83
Irq 84
Irq 85
Irq 86
Irq 87
Irq 88
Irq 89
Irq 90
Irq 91
Irq 92
Irq 93
Irq 94
Irq 95
Irq 96
Irq 97
Irq 98
Irq 99
Irq 100
Irq 101
Irq 102
Irq 103
Irq 104
Irq 105
Irq 106
Irq 107
Irq 108
Irq 109
Irq 110
Irq 111
Irq 112
Irq 113
Irq 114
Irq 115
Irq 116
Irq 117
Irq 118
Irq 119
Irq 120
Irq 121
Irq 122
Irq 123
Irq 124
Irq 125
Irq 126
Irq 127
Irq 128
Irq 129
Irq 130
Irq 131
Irq 132
Irq 133
Irq 134
Irq 135
Irq 136
Irq 137
Irq 138
Irq 139
Irq 140
Irq 141
Irq 142
Irq 143
Irq 144
Irq 145
Irq 146
Irq 147
Irq 148
Irq 149
Irq 150
Irq 151
Irq 152
Irq 153
Irq 154
Irq 155
Irq 156
Irq 157
Irq 158
Irq 159
Irq 160
Irq 161
Irq 162
Irq 163
Irq 164
Irq 165
Irq 166
Irq 167
Irq 168
Irq 169
Irq 170
Irq 171
Irq 172
Irq 173
Irq 174
Irq 175
Irq 176
Irq 177
Irq 178
Irq 179
Irq 180
Irq 181
Irq 182
Irq 183
Irq 184
Irq 185
Irq 186
Irq 187
Irq 188
Irq 189
Irq 190
Irq 191
Irq 192
Irq 193
Irq 194
Irq 195
Irq 196
Irq 197
Irq 198
Irq 199
Irq 200
Irq 201
Irq 202
Irq 203
Irq 204
Irq 205
Irq 206
Irq 207
Irq 208
Irq 209
Irq 210
Irq 211
Irq 212
Irq 213
Irq 214
Irq 215
Irq 216
Irq 217
Irq 218
Irq 219
Irq 220
;Irq 221
;Irq 222
;Irq 223
%endif
