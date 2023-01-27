#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 1000

void remove_newline(char *ptr)
{
    while (*ptr) {
        if (*ptr == '\n')
        *ptr = 0;
    else
        ptr++;
    }
}

int main()
{
    char nomeFuncao[3], rdi[4], rsi[4], rdx[4];
    
    char v1;
    int r;
    int quantidadeBytesNaPilha;
    char line[LINESZ];
    int count = 0;

    //Inserindo o template de um programa assembly
    printf(".data\n##Sem variáveis globais.\n");
    printf(".text\n");

    while (fgets(line, LINESZ, stdin) != NULL) {
        count++;
        remove_newline(line);

        // Verifica se line começa com 'function' (3 letras)
        if (strncmp(line, "function", 8) == 0) {
            sscanf(line, "function %s %s %s %s", nomeFuncao, rdi, rsi, rdx);  
            
            printf(".globl %s\n", nomeFuncao);
            printf("%s:\n", nomeFuncao);
            continue;
        }

        //Inicia a declaração de variáveis locais
        if (strncmp(line, "def", 3) == 0) {
      
            continue;
        }

        //Define uma variáveis local
        if (strncmp(line, "var", 3) == 0) {
      
            continue;
        }

        //Finaliza a declaração de variáveis locais
        if (strncmp(line, "enddef", 6) == 0) {
      
            continue;
        }

        //Atribuição FALTA AJUSTAR A POSIÇAO DA ATRIBUIÇÃO
        if (strncmp(line, "=", 1) == 0) {
      
            continue;
        }

        //Chamada de função FALTA AJUSTAR A POSIÇÃO DO CALL
        if(strncmp(line,"call", 4) == 0){

            //verificando se o primeiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdi, "pi", 2) == 0){
                //aloca %edi na pilha
                quantidadeBytesNaPilha +=4;    
            }
            else if(strncmp(rdi, "pa", 2) == 0){
                //aloca %rsi na pilha
                quantidadeBytesNaPilha +=8;    
            }
            
            //verificando se o segundo parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rsi, "pi", 2) == 0){
                //aloca %edi na pilha
                quantidadeBytesNaPilha +=4;    
            }
            else if(strncmp(rsi, "pa", 2) == 0){
                //aloca %rsi na pilha
                quantidadeBytesNaPilha +=8;    
            }

            //verificando se o terceiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdx, "pi", 2) == 0){
                //aloca %edi na pilha
                quantidadeBytesNaPilha +=4;    
            }
            else if(strncmp(rdx, "pa", 2) == 0){
                //aloca %rsi na pilha
                quantidadeBytesNaPilha +=8;    
            }    
        }

        //Acesso a um array
        if (strncmp(line, "get", 3) == 0) {
      
            continue;
        }

        //Condicional  
        if (strncmp(line, "if", 2) == 0) {
      
            continue;
        }

        //Fim condicional  
        if (strncmp(line, "endif", 5) == 0) {
      
            continue;
        }

        //Retorno da função
        if (strncmp(line, "return", 3) == 0) {
      
            continue;
        }

        //Fim da função
        if (strncmp(line, "end", 3) == 0) {
      
            continue;
        }
    }

    return 0;
}