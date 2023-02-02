#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 1000

int posicaoVariavelLocal;

struct CampoQueSeraAlocado{
    int quantidadeBytes;
    char nome[5];
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

int buscaValorSizeVetor(FILE * arquivoCopiaParaBuscarValorSizeDeVet, char * nomeVet){
    
    char line[LINESZ];
    int tamanhoPilha = 0, size;
    char nomeVariavel[4];   

    while (fgets(line, LINESZ, arquivoCopiaParaBuscarValorSizeDeVet) != NULL){
        
        if (strncmp(line, "vet", 2) == 0){
            sscanf(line, "vet %s size ci%d", nomeVariavel, &size);
            if(strstr(nomeVariavel, nomeVet)){
                return size;
            }
        }
    }
}

void alocaVariavel (char * nomeVariavelReceptora)
{
    int size;
    
    // variaveis inteiras
    if ((strncmp(nomeVariavelReceptora, "pi", 2) == 0 || strncmp(nomeVariavelReceptora, "vi", 2) == 0))
    {
        strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome, nomeVariavelReceptora);
        camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes = 4;
        posicaoVariavelLocal++;
    } else{
        size = buscaValorSizeVetor(fopen("./progParaTeste.blp", "rt"),nomeVariavelReceptora);
        
        if (size > 0)
        {
            strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome, nomeVariavelReceptora);
            camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes = 4 * size;
            posicaoVariavelLocal++;
        }
        else
        {
            printf("TAMANHO DE ARRAY NÃO DEVE SER NEGATIVO!");
            return;
        }
    }
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
            if(tamanhoPilha%16!=0){
                //O tamanho da pilha deve ser múltipla de 16.
                tamanhoPilha += 16-(tamanhoPilha%16);
            }
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

        char nomeFuncao[3],nomeVariavel[5],nomeVariavelReceptora[5];
        char rdi[4], rsi[4], rdx[4];
        char param1[4], param2[4], param3[4];
        char valor[4], condicao[4];   
        char operador1[4], operador2[4],operando[4];
        char nomeArray[4], destino[4], valorArray[4];
        int indice;
        int quantidadeBytesNaPilha, qntParametros, size;

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
                strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome,rdi);
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =4;    
                posicaoVariavelLocal++;
            }
            else if(strncmp(rdi, "pa", 2) == 0){
                //%rdi na pilha
                strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome,rdi);
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =8;    
                posicaoVariavelLocal++;
            }
            
            //verificando se o segundo parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rsi, "pi", 2) == 0){
                //%esi na pilha
                strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome,rsi);
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =4;    
                posicaoVariavelLocal++; 
            }
            else if(strncmp(rsi, "pa", 2) == 0){
                //%rsi na pilha
                strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome,rsi);
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =8;    
                posicaoVariavelLocal++; 
            }

            //verificando se o terceiro parametro veio com inteiro, ponteiro. Se vazio faz nada
            if(strncmp(rdx, "pi", 2) == 0){
                //%edx na pilha
                strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome,rdx);
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =4;    
                posicaoVariavelLocal++; 
            }
            else if(strncmp(rdx, "pa", 2) == 0){
                //%rdx na pilha
                strcpy(camposQueSeraoAlocados[posicaoVariavelLocal].nome,rdx);
                camposQueSeraoAlocados[posicaoVariavelLocal].quantidadeBytes =8;    
                posicaoVariavelLocal++;  
            }

            continue;
        }  

        //Atribuição
        if (strstr(line, "=")) {
            
            if (strstr(line, "+") || strstr(line, "-") || strstr(line, "*") || strstr(line, "/")){
                sscanf(line, "%s = %s %s %s",nomeVariavelReceptora, operador1,operando,operador2); 

                //operador 1 = variavel e operador2 = constante
                if((strncmp(operador1, "pi", 2) == 0 || strncmp(operador1, "pa", 2) == 0 || 
                strncmp(operador1, "vi", 2) == 0 || strncmp(operador1, "va", 2) == 0)
                && strncmp(operador2, "ci", 2) == 0 ){
                    if(strcmp(operando, "+") == 0){
                        printf("addl $%s,%s\n",operador2,operador1);
                        printf("movl %s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "-") == 0){
                        printf("subl $%s,%s\n",operador2,operador1);
                        printf("movl %s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "/") == 0){
                        printf("movl %s, %%ebx\n",operador2);
                        printf("xorl %%edx, %%edx\n");
                        printf("movl %s, %%eax\n",operador1);
                        printf("divl %%ebx, %%eax\n");
                        printf("movl %%eax, %s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "*") == 0){
                        printf("imul $%s,%s\n",operador2,operador1);
                        printf("movl %s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }
                }

                //operador 1 = constante e operador2 = variavel
                if((strncmp(operador2, "pi", 2) == 0 || strncmp(operador2, "pa", 2) == 0 || 
                strncmp(operador2, "vi", 2) == 0 || strncmp(operador2, "va", 2) == 0)
                && strncmp(operador1, "ci", 2) == 0 ){
                    if(strcmp(operando, "+") == 0){
                        printf("addl $%s,%s\n",operador1,operador2);
                        printf("movl %s,%%ecx\n",operador2);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "-") == 0){
                        printf("subl $%s,%s\n",operador1,operador2);
                        printf("movl %s,%%ecx\n",operador2);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "/") == 0){
                        printf("movl %s, %%ebx\n",operador1);
                        printf("xorl %%edx, %%edx\n");
                        printf("movl %s, %%eax\n",operador2);
                        printf("divl %%ebx, %%eax\n");
                        printf("movl %%eax, %s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "*") == 0){
                        printf("imul $%s,%s\n",operador1,operador2);
                        printf("movl %s,%%ecx\n",operador2);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }
                }

                //operador 1 = constante e operador2 = constante
                if(strncmp(operador1, "ci", 2) == 0 && strncmp(operador2, "ci", 2) == 0){
                    if(strcmp(operando, "+") == 0){
                        printf("movl $%s,%%ecx\n",operador2);
                        printf("addl $%s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "-") == 0){
                        printf("movl $%s,%%ecx\n",operador2);
                        printf("subl $%s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "/") == 0){
                        printf("movl %s, %%ebx\n",operador1);
                        printf("xorl %%edx, %%edx\n");
                        printf("movl %s, %%eax\n",operador2);
                        printf("divl %%ebx, %%eax\n");
                        printf("movl %%eax, %s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "*") == 0){
                        printf("movl $%s,%%ecx\n",operador2);
                        printf("imul $%s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }
                }

                //operador 1 = variavel e operador2 = varivael
                if((strncmp(operador1, "pi", 2) == 0 || strncmp(operador1, "pa", 2) == 0 || 
                strncmp(operador1, "vi", 2) == 0 || strncmp(operador1, "va", 2) == 0)
                && ((strncmp(operador2, "pi", 2) == 0 || strncmp(operador2, "pa", 2) == 0 || 
                strncmp(operador2, "vi", 2) == 0 || strncmp(operador2, "va", 2) == 0))){
                    if(strcmp(operando, "+") == 0){
                        printf("movl %s,%%ecx\n",operador2);
                        printf("addl %s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "-") == 0){
                        printf("movl %s,%%ecx\n",operador2);
                        printf("subl %s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "/") == 0){
                        printf("movl %s, %%ebx\n",operador1);
                        printf("xorl %%edx, %%edx\n");
                        printf("movl %s, %%eax\n",operador2);
                        printf("divl %%ebx, %%eax\n");
                        printf("movl %%eax, %s\n",nomeVariavelReceptora);
                    }else if(strcmp(operando, "*") == 0){
                        printf("movl %s,%%ecx\n",operador2);
                        printf("imul %s,%%ecx\n",operador1);
                        printf("movl %%ecx,%s\n",nomeVariavelReceptora);
                    }
                }
                
            //caso a atribuição seja o retorno de uma chamada de função    
            }else if(strstr(line, "call")){
                int i;

                sscanf(line, "%s = call %s %s %s %s",nomeVariavel, nomeFuncao, param1, param2, param3); 

                printf("\n");
                
                for(i = 0; i< posicaoVariavelLocal; i++){
                        printf("movq %s,-%d(%%rbp)\n",camposQueSeraoAlocados[i].nome, somaBytesAtePosicao(i));
                }

                qntParametros = sscanf(line, "call %s %s %s %s", nomeFuncao, param1, param2, param3);  

                if(qntParametros > 1) printf("movq %s,%%rdi\n",param1);
                if(qntParametros > 2) printf("movq %s,%%rsi\n",param2);
                if(qntParametros > 3) printf("movq %s,%%rdx\n",param3);
                if(qntParametros > 4) printf("Quantidade de parâmetros superior ao permitido.\n");
            
                printf("\ncall %s\n\n", nomeFuncao);
                
                //atribuição de retorno a variável receptora
                printf("\nmovl %%eax,%s\n",nomeVariavel);

                for(i = 0; i< posicaoVariavelLocal; i++){
                        printf("movq -%d(%%rbp), %s\n", somaBytesAtePosicao(i), camposQueSeraoAlocados[i].nome);
                }

            //quando é do modelo var = var ou var = constante    
            }else{
                sscanf(line, "%s = %s",nomeVariavelReceptora, operador1); 

                //var = var
                if((strncmp(operador1, "pi", 2) == 0 || strncmp(operador1, "pa", 2) == 0 || 
                strncmp(operador1, "vi", 2) == 0 || strncmp(operador1, "va", 2) == 0)){
                    printf("movl %s,%%ecx\n",operador1);
                    printf("movl %%ecx,%s\n",nomeVariavelReceptora);

                //var = constante    
                }else{
                    printf("movl %s,%s\n",operador1,nomeVariavelReceptora);
                }
            }


            //ARMAZENANDO A VARIAVEL RECEPTORA PARA ALOCAR AO FAZER O CALL
            alocaVariavel(nomeVariavelReceptora);

            continue;
        }

        //Chamada de função FALTA AJUSTAR A POSIÇÃO DO CALL
        if(strncmp(line,"call", 4) == 0){
            int i;
            printf("\n");
            
            for(i = 0; i< posicaoVariavelLocal; i++){
                    printf("movq %s,-%d(%%rbp)\n",camposQueSeraoAlocados[i].nome, somaBytesAtePosicao(i));
            }

            qntParametros = sscanf(line, "call %s %s %s %s", nomeFuncao, param1, param2, param3);  

            if(qntParametros > 1) printf("movq %s,%%rdi\n",param1);
            if(qntParametros > 2) printf("movq %s,%%rsi\n",param2);
            if(qntParametros > 3) printf("movq %s,%%rdx\n",param3);
            if(qntParametros > 4) printf("Quantidade de parâmetros superior ao permitido.\n");
        
            printf("\ncall %s\n\n", nomeFuncao);
            
            for(i = 0; i< posicaoVariavelLocal; i++){
                    printf("movq -%d(%%rbp), %s\n", somaBytesAtePosicao(i), camposQueSeraoAlocados[i].nome);
            }
            continue;
        }

        //Acesso a um array
        if (strncmp(line, "get", 3) == 0) {
            sscanf(line, "get %s index %d to %s",nomeArray, &indice, destino);
            printf("\nleaq %s, %%r11",nomeArray);
            printf("\nmovslq %d,%%r12",indice);
            printf("\nimuq $4, %%r12");
            printf("\naddq %%r11, %%r12");

            printf("\nmovq (%%r12), %s",destino);
            continue;
        }

        //Modificando a um array
        if (strncmp(line, "set", 3) == 0) {
            sscanf(line, "set %s index %d with %s",nomeArray, &indice, valorArray);
            
            printf("\nleaq %s, %%r11",nomeArray);
            printf("\nmovslq %d,%%r12",indice);
            printf("\nimuq $8, %%r12");
            printf("\naddq %%r11, %%r12");

            printf("\nmovq %s, (%%r12)",valorArray);
            continue;
        }

        //Condicional  
        if (strncmp(line, "if", 2) == 0) {
            sscanf(line, "if %s", condicao);
            printf("\n\ncmp $0,%s\n",condicao);
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

            continue;
        }

        //Fim da função
        if (strcmp(line, "end") == 0) {
            //prepara vetor para novos vampos da nova função
            for(int i = 0; i < 10; i++){
                camposQueSeraoAlocados[i].quantidadeBytes = 0;
                strcpy(camposQueSeraoAlocados[i].nome, "0");
            }
            continue;
        }
    }

    return 0;
}