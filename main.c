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
	scanf("%d", &nTasks);
	g = new_graph(nTasks+2);
	for(i=1;i<=nTasks;i++){
		scanf("%d", &n_tarefa);
		scanf("%d", &n_preced);
		int array[20], j=0;
		while(j<n_preced){
			scanf("%d", &array[j]);
			j++;
		}

		scanf("%d %d", &duration, &nTrab);
		dur[n_tarefa] = duration;
		trab[n_tarefa] = nTrab;
		if(n_preced == 0) 
			insert_new_arc(n_tarefa,nTasks+1, duration, nTrab,g);	// no nTasks+1 e o fim do projeto
		for(y=0;y<j;y++)
			insert_new_arc(n_tarefa, array[y], duration, nTrab, g);
	}

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
	for(i=1;i<=nTasks;i++){
		grE[i] = grauE[i];
		if(grauE[i] == 0)
			enqueue(i,q);
	}

	while(a != TRUE){
		v = dequeue(q);
		ARCO* adj = ADJS_NO(v,g);
		if( durMin < _ES[v] + dur[v]){
			durMin = _ES[v] + dur[v];
			v_f = v;
		}
		while(adj!=NULL){
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
		if(grauS[i] == 0)
			enqueue(i,q);
	}

	while(a!=TRUE) {
		int v = dequeue(q);
		ARCO * adjs = ADJS_NO(v,g);
		while(adjs!=NULL){
			int w = EXTREMO_FINAL(adjs);
			if(_LF[w] > _LF[v] - dur[v])
				_LF[w] = _LF[v] - dur[v];

			grauS[w] = grauS[w]-1;
			if(grauS[w]==0)
				enqueue(w,q);
			adjs = PROX_ADJ(adjs);
		}
		a = queue_is_empty(q);
	}
	
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
	int active_jobs[_ES[nTasks+1]];
	int  cont = 0;
	int i,j,max;

	for(i=1; i<=nTasks;i++){
		if( _ES[i] == 0 )
			cont += trab[i];
	}
	max = active_jobs[0] = cont;
	for(j=1;j <= _ES[nTasks+1];j++){
		for(i=1;i<=nTasks;i++){ 
			if( _ES[i] + dur[i] == j)
				cont -= trab[i];

			if( _ES[i] == j )
				cont += trab[i];

			active_jobs[j] = cont;

			if(cont > max)
				max = cont;
			
		}
	}
	return max;
}

int min_trabalhadores(GRAFO* g){
	int cont = 0, max, i, j;

	for(i=1; i<=nTasks; i++) {
		_LS[i] = _LF[i] - dur[i];
		if( _LS[i] == _ES[i] )
			printf("\t'ACTIVIDADE CRITICA' %d\n",i);
	}

	for(i=1; i<=nTasks;i++){
		if( _ES[i] == 0  && _LS[i] == _ES[i])
			cont += trab[i];
	}
	max = cont;
	for(j=1;j <= _ES[nTasks+1];j++)
		for(i=1;i<=nTasks;i++){ 
			if( _ES[i] + dur[i] == j && 
				_ES[i] == _LS[i] )
				cont -= trab[i];

			if( _ES[i] == j && 
				_ES[i] == _LF[i] )
				cont += trab[i];

			if(cont > max)
				max = cont;
			
		}
	return max;
}

int main(){
	int i;
	GRAFO *g = ler_construir_grafo();
	earl_star(g);
	escreveCaminho(prec[nTasks+1]);
	GRAFO *gt = transposto(g);
	lates_fini(gt);
	int t = trabalhadores_necessarios_ES(g);
	int mint = min_trabalhadores(g);

	printf("1.1) Duração minima do projeto: %d\n", durMin);
	printf("------------\n");
	printf("1.2) Minimo de TRABALHADORES necessarios em funcao do ES: %d\n",t);
	printf("------------\n");
	printf("1.3) Minimo de TRABALHADORES nas tarefas criticas: %d\n", mint);
	printf("------------\n");
	printf("Num vertices %d\n", NUM_VERTICES(g));
	printf("Num arcos %d\n", NUM_ARCOS(g));
	return 0;
}