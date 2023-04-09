        ORG 0000H
	    JMP BEG
    	ORG 0100H
BEG:    MOV 20H,#20H       ;给20H单元赋值
    	MOV 21H,#21H	   ;给21H单元赋值
     	MOV 30H,#30H	   ;给30H单元赋值
     	MOV 31H,#31H	   ;给31H单元赋值

        MOV R0,20H
    	CLR A
    	MOV A,R0          
    	ADDC A,30H
    	MOV 40H,A
    	MOV R0,21H
    	CLR A
    	MOV A,@R0
     	ADDC A,31H
	    MOV 41H,A

        MOV A,21H
		MOV B,31H
		MUL AB
		MOV R0,A      //low1
		MOV R1,B      //high1

		MOV A,21H
		MOV B,30H
		MUL AB
		MOV R2,A      //low2
		MOV R3,B      //high2

		MOV A,20H
		MOV B,31H
		MUL AB
		MOV R4,A      //low3
		MOV R5,B      //high3

		MOV A,20H
		MOV B,30H
		MUL AB
		MOV R6,A      //low4
		MOV R7,B      //high4

		MOV 53H,R0
		MOV A,R1
		ADD A,R2
		MOV R2,A
		MOV A,#00H
		ADDC A,R3
		CLR C
		MOV R3,A
		MOV A,#00H
		ADDC A,R7
		CLR C
		MOV R7,A

		MOV A,R2
		ADD A,R4
		MOV R4,A
		MOV 52H,R4
		MOV A,#00H
		ADDC A,R3
		CLR C
		MOV R3,A
		MOV A,#00H
		ADDC A,R7
		CLR C
		MOV R7,A
		
		MOV A,R3
		ADD A,R5
		MOV R5,A
		MOV A,#00H
		ADDC A,R7
		CLR C
		MOV R7,A 

		MOV A,R5
		ADD A,R6
		MOV 51H,A
		MOV A,#00H
		ADDC A,R7
		CLR C
		MOV 50H,A  
    	SJMP $
    	END 