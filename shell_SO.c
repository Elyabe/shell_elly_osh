/* Universidade Federal do Espírito Santo - Ufes
   Centro Universitário Norte do Espírito Santo - Ceunes
   Sistemas Operacionais 2018/1 - SO20181
   Prática Laboratorial 
   Construindo o proprio Shell simples
   Data: 12/05
   Autor: Elyabe Alves
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>

// Tamanho máximo da linha e memória de comandos, respectivamente
#define MAX_LINE 200 
#define QTD_MEMORIA 10

// Vetor de comandos do histórico
char history[QTD_MEMORIA][MAX_LINE];
unsigned topo_Hist = 0;

//Retorna 1 se existe comando executável
int commandInput( char** args );
// Exibe data atual do sistema
void dateSystem();
// Exibe histórico dos comandos por prioridade
void showHistory();


int main( void ) 
{
   	// QTD_MAX: Número máx de argumentos
   	// num_com: Número do comando no histórico;
    int QTD_MAX = MAX_LINE/2 + 1, status, num_com, should_run = 1;
    char *args[QTD_MAX]; 
    pid_t pid;
    
	while ( should_run ) 
	{
		printf("elly@osh> ");
		fflush(stdout);
        
        // Se houver comando 
        if ( commandInput(args) )
        {
			
			if ( !strcmp( args[0], "exit" ) )
               exit(0);  
            
            // Cria processo filho
            pid = fork();

            inicio_shell:
            if ( pid < 0 ) 
            {    
				printf("Função fork() falhou\n");
				exit(1);
			} else if ( pid == 0 ) 
			{  
				if( !strcmp( args[0], "date" ) )
				{ 
					dateSystem();  
					exit(1); 
				} else if ( !strcmp( args[0], "history" ) ) 
						{
							showHistory();
							exit(1);
						}else if ( args[0][0] == '!' )
				   		{
				   			// Utilizar, se existir, comando no histórico
				   			if ( topo_Hist > 1 )
				   			{ 
					   			// Último comando
					   			if ( !strcmp( args[0], "!!" ) )
					   				num_com = (topo_Hist - 1) % QTD_MEMORIA - 1;
					   			else
					   			{
					   				// Número do comando
					   				char copy_num[25];
					   				strcpy( copy_num, args[0] );
					   				copy_num[0] = '0';
					   				num_com =  atoi( copy_num ) - 1;
					   				if ( num_com <= topo_Hist /*&&  num_com > topo_Hist - QTD_MEMORIA*/ )
					   					num_com =  atoi( copy_num ) % QTD_MEMORIA - 1;
					   				else
					   				{
					   					printf("Não há comando correspondente no histórico!\n");
					   					exit(1);
					   				}
					   			}
					   			
				   				printf("\t%s\n", history[ num_com ] );
				   				
				   				// Se a execução do comando não criar Loop infinito
				   				if ( strcmp( history[ num_com ], "!!" ) )
				   				{
				   					strcpy( args[0], history[ num_com ] );
				   					goto inicio_shell;
				   				} else
			   						printf("Erro de comando cíclico\n");
				   			} else
				   				printf("Não há commandos no histórico!\n");
					   		exit(1);	
				   		} // Outro comando e deu erro
				   		else if ( execvp( *args, args ) < 0 ) 
						{  
							printf("Comando inválido!!!\n");
							exit(1);
						}
			}         
			else 
				while ( wait( &status ) != pid ); 
		}
	}

	return 0;
}

void showHistory()
{
	int i, num_com = topo_Hist;
	for ( i = (topo_Hist - 1) % QTD_MEMORIA; i >= 0; i--, num_com-- )
	 	printf("%d %s\n", num_com, history[i] );

	if ( topo_Hist >= QTD_MEMORIA )
		for ( i = QTD_MEMORIA - 1; i > topo_Hist % QTD_MEMORIA; i--, num_com-- )
			printf("%d %s\n", num_com, history[i] );
}

// Retorna 0 se não há comandos e 1 caso contrário
int commandInput( char **args )
{
	char command_line[MAX_LINE], *token; 
	int stop, ind_args;
	// stop: existe ou não commando executável
	// ind_args : índice dos argumentos

	stop = ind_args = 0;
	
	// Lê comando da entrada padrão e armazena no command_line
	fgets( command_line, MAX_LINE, stdin );
	
	// Se o comando for não vazio
	stop = ( strcmp(command_line,"") && command_line[0] != '\n' ); 
	if ( stop )
	{
		// Separa comando e seus parâmetros, se existirem
		token = strtok( command_line," \n" );
		while( token != NULL)
		{
			args[ind_args] = (char*)malloc( MAX_LINE*sizeof(char) );
			strcpy( args[ind_args++], token );
			strcpy( history[ topo_Hist % QTD_MEMORIA ], token );
			topo_Hist++;
			token = strtok(NULL," \n");	
		}

		//Último argumento é NULL
		args[ind_args] = NULL;
	}

	return stop;
}

void dateSystem()
{
	printf("DATA : %s HORA: %s\n",__DATE__,__TIME__);
}
