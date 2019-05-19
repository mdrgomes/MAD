#include <stdio.h>
#include "grafos2.h"
#include "queue.h"
#include "string.h"
#define IN 1
#define OUT 0

//variaveis globais
static int nTasks;  
static int v_f; 
static int durMin = -1;
static int grauE[MAXVERTS];
static int grauS[MAXVERTS];
static int _ES[MAXVERTS];
static int _LF[MAXVERTS];
static int _LS[MAXVERTS];
static int _FT[MAXVERTS];
static int _FL[MAXVERTS];
static int prec[MAXVERTS];
static int dur[MAXVERTS];
static int trab[MAXVERTS];

GRAFO * ler_construir_grafo(){
	GRAFO *g;
	int n_tarefa , nTrab,n_preced;
	int duration, i, j, y;
	//printf("Enter number of tasks\n");
	scanf("%d", &nTasks);
	//int array[20];
	g = new_graph(nTasks+1);
	for(i=1;i<=nTasks;i++){
		//printf("Enter Task\n");
		scanf("%d", &n_tarefa);
		//printf("Enter number of precedents\n");
		scanf("%d", &n_preced);

		//array com precedencias 
		int array[20], j=0;
		while(j<n_preced){
			//printf("precedente\n");
			scanf("%d", &array[j]);
			j++;
		}
		//printf("duração e numero de trablhadores\n");
		scanf("%d %d", &duration, &nTrab);
		dur[n_tarefa] = duration;
		trab[n_tarefa] = nTrab;
		if(n_preced == 0) 
			insert_new_arc(n_tarefa,nTasks+1, duration, nTrab,g);	// no nTasks+1 e o fim do projeto
		for(y=0;y<j;y++)
			insert_new_arc(n_tarefa, array[y], duration, nTrab, g);
	}

	//instanciar arrays de grau de entrada e de saida
	for(i=1;i<=nTasks;i++){
		_ES[i]=0;
		prec[i]=-99;
		grauE[i]=0;
		grauS[i]=0;	
	}
	for(i=1;i<=nTasks;i++)
		for(j=1;j<=nTasks;j++)
			if(find_arc(i,j,g)!=NULL){
				grauE[j]=grauE[j]+1;
				grauS[i]=grauS[i]+1;
			}
	return g;
}


		/*   PERGUNTA 1.2 //usar o earliest start 

		int events_matrix[nTasks][MAXVERTS];
		for(i=0;i<nTasks;i++)
		for(j=0;j<MAXVERTS;j++)
			events_matrix[i][j] = 0;
		
		ARCO* adj = ADJS_NO(i,g);
		for(j=0; j<VALOR1_ARCO(adj);j++){
			events_matrix[i][j]= VALOR2_ARCO(adj);
			printf("---events_matrix[%d][%d] = %d\n", i, j, events_matrix[k][j]);
		}
		*/
	

//concluida
void earl_star(GRAFO *g){
	int i,j,k=0;
	QUEUE *q = mk_empty_queue(nTasks);
	v_f = 0;
	BOOL a = FALSE;
	int v, max_trabalhadores_activos = 0;
	ARCO* arc ;

	//tarefas iniciais (sem precedentes)
	for(i=1;i<=nTasks;i++){
		//printf("grauE[%d]=%d | grauS[%d]=%d\n",i,grauE[i],i,grauS[i]);
		if(grauE[i] == 0)
			enqueue(i,q);
	}

	//algoritmo ES
	while(a != TRUE){
		v = dequeue(q);
		//printf("tarefa %d\n",v);
		ARCO* adj = ADJS_NO(v,g);
		if( durMin < _ES[v] + VALOR1_ARCO(adj)){
			durMin = _ES[v] + VALOR1_ARCO(adj);
			v_f = v;
		}	
		//max_trabalhadores_activos += VALOR2_ARCO(adj);
		while(adj!=NULL){
			//printf("entrei\n");
			int w = EXTREMO_FINAL(adj);
			if(_ES[w] < _ES[v] + VALOR1_ARCO(adj)){
				_ES[w] = _ES[v] + VALOR1_ARCO(adj);
				prec[w] = v;
			}
			grauE[w] = grauE[w]-1; 
			if(grauE[w] == 0)
				enqueue(w,q);
			adj = PROX_ADJ(adj);
		}
		a = queue_is_empty(q);
	}

	//for(i=1;i<=nTasks;i++)
	//	printf("\tprec[%d] = %d  ES[%d] = %d\n",i,prec[i],i,_ES[i]); 
	printf("duracao minima do projeto: %d\n",durMin);
}

GRAFO * transposto(GRAFO *g){
	int n = NUM_VERTICES(g);
	GRAFO *gt = new_graph(nTasks+1);
	int i;
	for(i=1;i<=nTasks;i++){
		ARCO *adj = ADJS_NO(i,g);
		if(grauE[i] == 0){
			insert_new_arc(i,nTasks+1,VALOR1_ARCO(adj),VALOR2_ARCO(adj),gt);
		}
		while(adj!=NULL){
			//printf("alerta\n");
			int w = EXTREMO_FINAL(adj);
			//printf("I:%d   w:%d\n",i, w );
			insert_new_arc(w,i,VALOR1_ARCO(adj), VALOR2_ARCO(adj), gt);
			adj = PROX_ADJ(adj);
		}
	}
	
	return gt;
}

void lates_fini(GRAFO* g){
	int i, j, w;
	GRAFO* gt = transposto(g);
	QUEUE * q = mk_empty_queue(nTasks);
	BOOL a = FALSE;
	for(i=1;i<=nTasks;i++){
		_LF[i]= durMin;
		if(grauS[i] == 0)
			enqueue(i,q);
	}
	while(a!=TRUE){
		int v = dequeue(q);
		ARCO * adjs = ADJS_NO(v,gt);
		//printf("v: %d\n", v);
		while(adjs != NULL){
			w = EXTREMO_FINAL(adjs);
			if(_LF[w] > _LF[v] - VALOR1_ARCO(adjs)){
				_LF[w]= _LF[v] - VALOR1_ARCO(adjs);
				//printf("LF[%d]: %d \n",w,_LF[w] );
			}
			grauS[w]=grauS[w]-1;
			if(grauS[w]==0){
				//printf("alerta\n");
				enqueue(w,q);
			}
			adjs = PROX_ADJ(adjs);
		}

		a = queue_is_empty(q);
	}
	for(i=1;i<=nTasks;i++)
        printf("\tprec[%d] = %d  LF[%d] = %d   ES[%d] = %d\n",i,prec[i],i,_LF[i], i,_ES[i]);
        
}

//concluida
void escreveCaminho(int v_f){
	int j=0, i=1, k = 0;
	int* caminho = (int *) malloc(nTasks * sizeof(int));
	for(i=0; i<nTasks;i++)
		caminho[i] = 0;

	caminho[0] = v_f;
	while( prec[v_f] != -99 ) {
		caminho[++k] = prec[v_f];
		v_f = prec[v_f];
	}

	printf("---CAMINHO---\n");
	printf("%d", caminho[k]);
	for(i=k-1; i>=0; i--) 
		printf(" - %d", caminho[i]);
	printf("\n");
}

void lates_start(GRAFO *g){
	//LS = LF[ij] - Dij
	int i;	
	for(i=1;i<=nTasks;i++){
		ARCO * adjs = ADJS_NO(i,g);
		_LS[i]= _LF[i] - VALOR1_ARCO(adjs);
		//printf("LS[%d] : %d\n", i, _LS[i]);
	}
}

void folga_total(GRAFO *g){
	int i;
	for(i=1;i<=nTasks;i++){
		_FT[i]= _LS[i] - _ES[i];
	}
}

void folga_livre(GRAFO *g){
	int i;
	for (i = 1; i<=nTasks; ++i){
		ARCO *adjs=ADJS_NO(i,g);
		while(adjs!=NULL){
			int w = EXTREMO_FINAL(adjs);
			_FL[i] = _ES[w] - ( _ES[i] - VALOR1_ARCO(adjs));
			adjs = PROX_ADJ(adjs);
		}
	}
}

void number_works(GRAFO *g){//pergunta 1.2 //algoritmos do earliest start
	int i, j, w;
	int flag[MAXVERTS];
	int workers=0;
	int time=0;
	BOOL a= FALSE;
	QUEUE *q = mk_empty_queue(nTasks);

	for(i=1;i<=nTasks;i++){
		if(_ES[i]==0)
			enqueue(i,q);
	}
	//fazer contagem com o numeor de trabalhadores com ES
	int v;
	//int a =0;
	/*n_trab=-1;


	for(i=0;i<durMin;i++){
		
	}*/
	printf("%d\n", workers);
	
}

int main(){
	GRAFO *g = ler_construir_grafo();//concluida
	earl_star(g);
	lates_fini(g);
	lates_start(g);
	folga_total(g);
	folga_livre(g);
	
	//number_works(g);
	//printf("Num vertices %d\n", NUM_VERTICES(g));
	//printf("Num arcos %d\n", NUM_ARCOS(g));
	return 0;
}

//atividade critica FT = FL 
