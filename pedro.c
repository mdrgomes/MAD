#include <stdio.h>
#include "grafos2.h"
#include "queue.h"
#include "string.h"


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

//concluida
GRAFO * ler_construir_grafo(){
	GRAFO *g;
	int n_tarefa , nTrab,n_preced;
	int duration, i, j, y;
	//printf("Enter number of tasks\n");
	scanf("%d", &nTasks);
	//int array[20];
	g = new_graph(nTasks+2);
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
	for(i=0;i<=nTasks+1;i++){
		_ES[i]=0;
		_LS[i]=0;
		_FT[i]=0;
		_FL[i]=0;
		prec[i]=-99;
		grauE[i]=0;
		grauS[i]=0;	
	}
	for(i=1;i<=nTasks;i++){
		for(j=1;j<=nTasks;j++)
			if(find_arc(i,j,g)!=NULL){
				grauE[j]=grauE[j]+1;
				grauS[i]=grauS[i]+1;
			}
		if( grauE[i] == 0 )
			insert_new_arc(0, i, 0, 0, g);
		if( grauS[i] == 0)
			insert_new_arc(i, 0, 0, 0, g);
		printf("dur[%d] = %d\n",i, dur[i]);
	}

	return g;
}


//concluida
		void earl_star(GRAFO *g){
			int i,j,k=0;
			QUEUE *q = mk_empty_queue(nTasks);
			v_f = 0;
			BOOL a = FALSE;
			int v, max_trabalhadores_activos = 0;
			ARCO* arc ;
			int grE[nTasks+1];
	//tarefas iniciais (sem precedentes)
			for(i=1;i<=nTasks;i++){
				//printf("grauE[%d]=%d | grauS[%d]=%d\n",i,grauE[i],i,grauS[i]);
				grE[i] = grauE[i];
				if(grauE[i] == 0)
					enqueue(i,q);
			}

	//algoritmo ES
			while(a != TRUE){
				v = dequeue(q);
		//printf("tarefa %d\n",v);
				ARCO* adj = ADJS_NO(v,g);
				if( durMin < _ES[v] + dur[v]){
					durMin = _ES[v] + dur[v];
					v_f = v;
				}
				while(adj!=NULL){
			//printf("entrei\n");
					int w = EXTREMO_FINAL(adj);
					if(_ES[w] < _ES[v] + VALOR1_ARCO(adj)){
						_ES[w] = _ES[v] + VALOR1_ARCO(adj);
						prec[w] = v;
					}
					grE[w] = grE[w]-1; 
					if(grE[w] == 0)
						enqueue(w,q);
					adj = PROX_ADJ(adj);
				}
				a = queue_is_empty(q);
			}
/*
			for(i=1;i<=nTasks+1;i++)
				printf("\tprec[%d] = %d  ES[%d] = %d\n",i,prec[i],i,_ES[i]); 
*/			printf("duracao minima do projeto: %d \n",_ES[nTasks+1]);
		}

		GRAFO * transposto(GRAFO *g){
			GRAFO *gt = new_graph(nTasks+1);
			int i;

			for(i=0;i<=nTasks;i++){
				ARCO *adj = ADJS_NO(i,g);
				while(adj!=NULL){
					int w = EXTREMO_FINAL(adj);
					insert_new_arc(w,i,dur[i], trab[i], gt);
					adj = PROX_ADJ(adj);
				}
			}
			return gt;
		}

		void lates_fini(GRAFO* g){
			int i, j, w;
			QUEUE * q = mk_empty_queue(nTasks);
			BOOL a = FALSE;

			for(i=1;i<=nTasks;i++){
				_LF[i]= _ES[nTasks+1];
				//printf("grauS[%d]= %d\n", i, grauS[i]);
				if(grauS[i] == 0)
					enqueue(i,q);
			}
		//	printf("um!\n");
			while(a!=TRUE) {
				int v = dequeue(q);
		//printf("valor de v %d - ", v);
				ARCO * adjs = ADJS_NO(v,g);
				while(adjs!=NULL){
					int w = EXTREMO_FINAL(adjs);
			//printf("valor de w %d\n", w);
					if(_LF[w] > _LF[v] - dur[v])
						_LF[w] = _LF[v] - dur[v];
			
					grauS[w] = grauS[w]-1;
					if(grauS[w]==0)
						enqueue(w,q);
					adjs = PROX_ADJ(adjs);
				}
				a = queue_is_empty(q);
			}
			for(i=1;i<=nTasks;i++)
				printf("LF[%d] = %d. ES[%d]= %d\n",i,_LF[i], i, _ES[i]);

		}

//concluida
		void escreveCaminho(int v_final){
			int j=0, i=1, k = 0;
			int* caminho = (int *) malloc(nTasks * sizeof(int));
			for(i=0; i<nTasks;i++)
				caminho[i] = 0;

			caminho[0] = v_final;
			while( prec[v_final] != -99 ) {
				caminho[++k] = prec[v_final];
				v_final = prec[v_final];
			}

			printf("---CAMINHO---\n");
			printf("%d", caminho[k]);
			for(i=k-1; i>=0; i--) 
				printf(" - %d", caminho[i]);
			printf("\n");
		}

//   PERGUNTA 1.2 concluida
		int trabalhadores_necessarios_ES(GRAFO *g) {
			int i,j,max = 0, cont;
			int events_matrix[nTasks+1][MAXVERTS];
			for(i=0;i<=nTasks;i++)
				for(j=0;j<MAXVERTS;j++)
					events_matrix[i][j] = 0;

			for(i=1; i<=nTasks;i++){
				//printf("entrei %d\n",i);
				ARCO* adj = ADJS_NO(i,g);

				for(j=_ES[i]; j < _ES[i] + VALOR1_ARCO(adj);j++){
					events_matrix[i][j]= VALOR2_ARCO(adj);
					//printf("---events_matrix[%d][%d] = %d\n", i, j, events_matrix[i][j]);
				}
			}

			for(j=0;j<MAXVERTS;j++){
				cont = 0;
				for(i=1;i<=nTasks;i++){
						cont += events_matrix[i][j];
					//printf("---events_matrix[%d][%d] = %d\n", i, j, events_matrix[i][j]);
				}

				//printf("cont=%d\n",cont);
 				if (max < cont)
 					max = cont;
			}
		return max;
		}

		int min_trabalhadores(GRAFO* g){
			

			return 0;
		}

			int main(){
	GRAFO *g = ler_construir_grafo();//concluida
	earl_star(g);
	escreveCaminho(prec[nTasks+1]);
	int t = trabalhadores_necessarios_ES(g);
	int mint = min_trabalhadores(g);
	GRAFO *gt = transposto(g);
	lates_fini(gt);

	printf("TRABALHADORES : %d\n",t);
	printf("Minimo de TRABALHADORES: %d\n", mint);

	printf("Num vertices %d\n", NUM_VERTICES(g));
	printf("Num arcos %d\n", NUM_ARCOS(g));
	return 0;
}