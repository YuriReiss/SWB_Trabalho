#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 1000

struct CampoQueSeraAlocado{
    int quantidadeBytes;
    char * nome;
} camposQueSeraoAlocados[20];

//Auxilia na escrita da alocação na pilha onde se precisa saber menos quantos bytes 
//após o rbp são necessários para alocar um item.
int somaBytesAtePosicao (int posicao)
{
    int qntBytes = 0;
    for(int i = 0; i<= posicao; i++){
        qntBytes+=camposQueSeraoAlocados[i].quantidadeBytes;
    }
    return qntBytes;
}

//Soma a quantidade de bytes para alocar os parâmetros recebidos na função
//mais os bytes da variáveis locais. A função também desenha a pilha.
int calculaTamanhoPilhaParaFuncao (FILE * arquivoCopiaParaCalcularQuantidadeDeBytesParaAlocar, char * nomeFuncao)
{
    char line[LINESZ];
    int tamanhoPilha = 0, size;
    char nome[3], rdi[4], rsi[4], rdx[4], nomeVariavel[4];   

    while (fgets(line, LINESZ, arquivoCopiaParaCalcularQuantidadeDeBytesParaAlocar) != NULL){
        
        if (strncmp(line, "function", 8) == 0) {
        
            sscanf(line, "function %s %s %s %s", nome, rdi, rsi, rdx);
            
            if(strcmp(nome,nomeFuncao) == 0){
                printf("\n\n##A PILHA DA FUNÇÃO %s:\n",nome);

                //verificando se o primeiro parametro veio com inteiro ou ponteiro. Se vazio faz nada
                if(strncmp(rdi, "pi", 2) == 0){
                    tamanhoPilha +=4;
                    printf("#%s: -%d\n",rdi,tamanhoPilha);
                }
                else if(strncmp(rdi, "pa", 2) == 0){
                    tamanhoPilha +=8;
                    printf("#%s: -%d\n",rdi,tamanhoPilha);
                } 
                                
                //verificando se o segundo parametro veio com inteiro ou ponteiro. Se vazio faz nada
                if(strncmp(rsi, "pi", 2) == 0){
                    tamanhoPilha +=4;
                    printf("#%s: -%d\n",rsi,tamanhoPilha);
                }
                else if(strncmp(rsi, "pa", 2) == 0){
                    tamanhoPilha +=8;
                    printf("#%s: -%d\n",rsi,tamanhoPilha);
                }

                //verificando se o terceiro parametro veio com inteiro ou ponteiro. Se vazio faz nada
                if(strncmp(rdx, "pi", 2) == 0){
                    tamanhoPilha +=4;
                    printf("#%s: -%d\n",rdx,tamanhoPilha);
                }else if(strncmp(rdx, "pa", 2) == 0){
                    tamanhoPilha+=8;
                    printf("#%s: -%d\n",rdx,tamanhoPilha);
                }
            }
        }
        if (strncmp(line, "var", 3) == 0 && strcmp(nome,nomeFuncao) == 0){
            sscanf(line, "var %s", nomeVariavel);
            tamanhoPilha +=4;
            printf("#%s: -%d\n",nomeVariavel,tamanhoPilha);

        }else if (strncmp(line, "vet", 3) == 0 && strcmp(nome,nomeFuncao) == 0){
            sscanf(line, "vet %s size ci%d", nomeVariavel, &size);

            if(size>0){
                tamanhoPilha += 4*size;
            }else{
                printf("TAMANHO DE ARRAY NÃO DEVE SER NEGATIVO!");
                return 0;
            }

            printf("#%s: -%d\n",nomeVariavel,tamanhoPilha);
        }else if(strncmp(line, "end", 3) == 0 && strcmp(nome,nomeFuncao) == 0){
            return tamanhoPilha;
        }
    }
}

int main()
{
    char line[LINESZ];
    int count = 0;

    //Inserindo o template de um programa assembly
    printf(".data\n\n\n");
    printf(".text\n");

    while (fgets(line, LINESZ, stdin) != NULL) {

        char nomeFuncao[3], rdi[4], rsi[4], rdx[4], nomeVariavel[5],valor[4], condicao[4];   
        char v1;
        int r, quantidadeBytesNaPilha, qntParametros, posicaoVariavelLocal, size;

        count++;

        // Verifica se line começa com 'function' 
        if (strncmp(line, "function", 8) == 0) {

            sscanf(line, "function %s %s %s %s", nomeFuncao, rdi, rsi, rdx);  
            
            printf("\n.globl %s\n", nomeFuncao);
            printf("%s:\n", nomeFuncao);

            printf("pushq %%rbp\nmovq %%rsp, %%rbp");

            printf("\nsubq -%d,%%rsp\n\n", calculaTamanhoPilhaParaFuncao(fopen("./progParaTeste.blp", "rt"), nomeFuncao));

            posicaoVariavelLocal = 0;

            //verificando se o primeiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdi, "pi", 2) == 0){
                //%edi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = rdi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =4;    
                posicaoVariavelLocal++;
            }
            else if(strncmp(rdi, "pa", 2) == 0){
                //%rdi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = rdi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =8;    
                posicaoVariavelLocal++;
            }
            
            //verificando se o segundo parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rsi, "pi", 2) == 0){
                //%esi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = rsi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =4;    
                posicaoVariavelLocal++; 
            }
            else if(strncmp(rsi, "pa", 2) == 0){
                //%rsi na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = rsi;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =8;    
                posicaoVariavelLocal++; 
            }

            //verificando se o terceiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdx, "pi", 2) == 0){
                //%edx na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = rdx;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =4;    
                posicaoVariavelLocal++; 
            }
            else if(strncmp(rdx, "pa", 2) == 0){
                //%rdx na pilha
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = rdx;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =8;    
                posicaoVariavelLocal++;  
            }

            continue;
        }

        //variaveis inteiras
        if (strncmp(line, "var", 3) == 0){
            sscanf(line, "var %s", nomeVariavel);
            camposQueSeraoAlocados[posicaoVariavelLocal].nome = nomeVariavel;
            camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes = 4;
            posicaoVariavelLocal++;
            printf("movq %s,-%d(%%rbp)\n",nomeVariavel,somaBytesAtePosicao(posicaoVariavelLocal));
            
            continue;
        }
        
        //variaveis de arrays inteiros
        if (strncmp(line, "vet", 3) == 0){
            sscanf(line, "vet %s size ci%d", nomeVariavel, &size);

            if(size>0){
                camposQueSeraoAlocados[posicaoVariavelLocal].nome = nomeVariavel;
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes = 4*size;
                posicaoVariavelLocal++;
                printf("movq %s,-%d(%%rbp)\n",nomeVariavel,somaBytesAtePosicao(posicaoVariavelLocal));
            }else{
                printf("TAMANHO DE ARRAY NÃO DEVE SER NEGATIVO!");
                return 0;
            }

            continue;
        }   

        //Atribuição FALTA AJUSTAR A POSIÇAO DA ATRIBUIÇÃO
        if (strncmp(line, "=", 1) == 0) {
      
            continue;
        }

        //Chamada de função FALTA AJUSTAR A POSIÇÃO DO CALL
        if(strncmp(line,"call", 4) == 0){
            int i;
            printf("\n");
            
            for(i = 0; i< posicaoVariavelLocal; i++){
                    printf("movq %s,-%d(%%rbp)\n",camposQueSeraoAlocados[i].nome, somaBytesAtePosicao(i));
            }

            qntParametros = sscanf(line, "call %s %s %s %s", nomeFuncao, rdi, rsi, rdx);  

            if(qntParametros > 1) printf("movq %s,%%rdi\n",rdi);
            if(qntParametros > 2) printf("movq %s,%%rsi\n",rsi);
            if(qntParametros > 3) printf("movq %s,%%rdx\n",rdx);
            if(qntParametros > 4){
                printf("Quantidade de parâmetros superior ao permitido.\n");
            }

            printf("call %s\n\n", nomeFuncao);
            
            for(i = 0; i< posicaoVariavelLocal; i++){
                printf("movq -%d(%%rbp), %s\n", somaBytesAtePosicao(i), camposQueSeraoAlocados[i].nome);
            }
            continue;
        }

        //Acesso a um array
        if (strncmp(line, "get", 3) == 0) {
      
            continue;
        }

        //Condicional  
        if (strncmp(line, "if", 2) == 0) {
            sscanf(line, "if %s", condicao);
            printf("cmp $0,%s\n",condicao);
            printf("je endif\n");
            continue;
        }

        //Fim condicional  
        if (strncmp(line, "endif", 5) == 0) {
            printf("\nendif:\n\n");
            continue;
        }

        //Retorno da função
        if (strncmp(line, "return", 3) == 0) {
      
            int qntAtribuicao = sscanf(line, "return %s", valor);
            
            if(qntAtribuicao == 1){
                if(strncmp(line, "pa", 2) == 0 || strncmp(line, "va", 2) == 0){
                    printf("\n\nmovq %s,%%rax\n\n",valor);
                }else if(strncmp(line, "pi", 2) == 0 || strncmp(line, "vi", 2) == 0){
                    printf("\n\nmovl %s,%%eax\n\n",valor);
                }else{
                    sscanf(valor, "ci%s", valor);
                    printf("\n\nmovl $%s,%%eax\n\n",valor);
                }
            }

            break;
        }

        //Fim da função
        if (strcmp(line, "end") == 0) {
            //prepara vetor para novos vampos da nova função
            for(int i = 0; i < 10; i++){
                camposQueSeraoAlocados[i].quantidadeBytes = 0;
                camposQueSeraoAlocados[i].nome = NULL;
            }
            continue;
        }
    }

    return 0;
}