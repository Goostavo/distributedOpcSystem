//Trabalho de Sistemas Distribuídos para automação
//main.c
//
//Trabalho Prático de Sistemas Distribuídos Para Automação 2011/02
//Gustavo Barreto Garcia		2006048133
//Rafael Gilmar Ribeiro Gurgel	2006048192

#include <iostream>
#include "Opcclass.h"

int main (void)
{
	std::cout << "Hello World!" << std::endl;
	Opcclass *Opc_tp = new Opcclass();			//Instancia Classe OPC
	Opc_tp->AddGroup();
	Opc_tp->AddItem();
	system("pause");
	return 0;
}