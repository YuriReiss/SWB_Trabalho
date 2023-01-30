#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 1000

struct CampoQueSeraAlocado{
    int quantidadeBytes;
    char * valor;
} camposQueSeraoAlocados[10];

void remove_newline(char *ptr)
{
    while (*ptr) {
        if (*ptr == '\n')
        *ptr = 0;
    else
        ptr++;
    }
}

//auxilia na escrita da alocação na pilha 
//onde se precisa saber menos quantos bytes 
//após o rbp são necessários para alocar um item
int somaBytesAtePosicao (int posicao)
{
    int qntBytes = 0;
    for(int i = 0; i<= posicao; i++){
        qntBytes+=camposQueSeraoAlocados[i].quantidadeBytes;
    }
    return qntBytes;
}

int main()
{
    char line[LINESZ];
    int count = 0;

    //Inserindo o template de um programa assembly
    printf(".data\n\n\n");
    printf(".text\n");

    while (fgets(line, LINESZ, stdin) != NULL) {

        char nomeFuncao[3], rdi[4], rsi[4], rdx[4];   
        char v1;
        int r, quantidadeBytesNaPilha, qntParametros, posicaoVariavelLocal;

        count++;
        remove_newline(line);

        // Verifica se line começa com 'function' (3 letras)
        if (strncmp(line, "function", 8) == 0) {

            sscanf(line, "function %s %s %s %s", nomeFuncao, rdi, rsi, rdx);  
            
            printf(".globl %s\n", nomeFuncao);
            printf("%s:\n", nomeFuncao);

            printf("pushq %%rbp\n movq %%rsp, %%rbp\n\n");

            posicaoVariavelLocal = 0;

            //verificando se o primeiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdi, "pi", 2) == 0){
                //%edi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].valor = rdi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes +=4;    
                posicaoVariavelLocal++;
            }
            else if(strncmp(rdi, "pa", 2) == 0){
                //%rdi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].valor = rdi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes +=8;    
                posicaoVariavelLocal++;
            }
            
            //verificando se o segundo parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rsi, "pi", 2) == 0){
                //%esi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].valor = rsi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes +=4;    
                posicaoVariavelLocal++; 
            }
            else if(strncmp(rsi, "pa", 2) == 0){
                //%rsi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].valor = rsi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes +=8;    
                posicaoVariavelLocal++; 
            }

            //verificando se o terceiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdx, "pi", 2) == 0){
                //%edx na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].valor = rdx;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes +=4;    
                posicaoVariavelLocal++; 
            }
            else if(strncmp(rdx, "pa", 2) == 0){
                //%rdx na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].valor = rdx;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes +=8;    
                posicaoVariavelLocal++;  
            }

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

            for(int i = 0; camposQueSeraoAlocados[i].valor != NULL; i++){
                    printf("movq %s,-%d(%%rbp)\n\n",camposQueSeraoAlocados[i].valor, somaBytesAtePosicao(i));
            }

            qntParametros = sscanf(line, "call %s %s %s %s", nomeFuncao, rdi, rsi, rdx);  

            if(qntParametros > 1) printf("movq %s,%%rdi\n",rdi);
            if(qntParametros > 2) printf("movq %s,%%rsi\n",rsi);
            if(qntParametros > 3) printf("movq %s,%%rdx\n",rdx);
            if(qntParametros > 4){
                printf("Quantidade de parâmetros superior ao permitido.\n");
            }

            printf("call %s\n\n", nomeFuncao);

            for(int i = 0; camposQueSeraoAlocados[i].valor != NULL; i++){
                printf("movq -%d(%%rbp), %s\n\n", somaBytesAtePosicao(i), camposQueSeraoAlocados[i].valor);
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
            //prepara vetor para novos vampos da nova função
            for(int i = 0; i < 10; i++){
                camposQueSeraoAlocados[i].quantidadeBytes = 0;
                camposQueSeraoAlocados[i].valor = NULL;
            }
            continue;
        }
    }

    return 0;
}