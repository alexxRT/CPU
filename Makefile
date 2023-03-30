CPU_COMPILE:
	@echo -------------------------------------------------------------------
	g++ CPU_v_1.cpp Stack.cpp Memory.cpp TextFunc.cpp ILOVEDEBUG.cpp -o CPU
	@echo -------------------------------------------------------------------
#commands are needed to be left without "@"

ASM_COMPILE:
	@echo -------------------------------------------------------------------------
	g++ assembler_v_1.cpp Stack.cpp Memory.cpp TextFunc.cpp ILOVEDEBUG.cpp -o asm
	@echo -------------------------------------------------------------------------

arseny:
	@echo ars_loh

ded:
	@echo ded_adors_piton