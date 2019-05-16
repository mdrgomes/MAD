#include <stdio.h>
#include "grafos2.h"
#include "queue.h"

//typedef enum {FALSE,TRUE} BOOL;

static int nTasks; //colocar no main 

GRAFO * ler_construir_grafo(){
	GRAFO *g;
	int n_tarefa , nTrab,n_preced;
	int duration, i;
	printf("Enter number of tasks\n");
	scanf("%d", &nTasks);
	//int array[20];
	g = new_graph(nTasks);
	for(i=1;i<=nTasks;i++){
		printf("Enter Task\n");
		scanf("%d", &n_tarefa);
		printf("Enter number of precedents\n");
		scanf("%d", &n_preced);
		//array com precedencias 
		int array[20], j=0;
		while(n_preced>0){
			printf("precedente\n");
			scanf("%d", &array[j]);
			j++;
			n_preced--;
		}
		printf("duração e numero de trablhadores\n");
		scanf("%d %d", &duration, &nTrab);
		int y;
		for(y=0;y<j;y++){
			insert_new_arc(n_tarefa, array[y], duration, nTrab, g);
		}
	}
	return g;
}

void earl_star(GRAFO *g, int n_Tasks){
	int i,j;
	int GRauE[MAXVERTS], ES[MAXVERTS], Prec[MAXVERTS];
	for(i=1;i<=nTasks;i++){
		ES[i]=0;
		Prec[i]=NULL;
		GRauE[i]=0;
	}
	for(i=1;i<=NUM_ARCOS(g);i++){
		for(j=1;j<=NUM_ARCOS(g);j++){
			if(find_arc(i,j,g)!=NULL)
				GRauE[j]=GRauE[j]+1;
		}
	}
	QUEUE *S = (QUEUE *) malloc(sizeof(QUEUE));
	S = mk_empty_queue(nTasks);
	int DurMin =-1;
	int v_f=0;
//falta enqueue
	BOOL a= TRUE;
	while(a!=FALSE){
		int v = dequeue(S);
		if(DurMin<ES[v]){
			DurMin = ES[v];
			v_f=v;
		}
		ARCO* adj = ADJS_NO(v,g);
		while(adj!=NULL){
			int w = EXTREMO_FINAL(adj);
			if(ES[w]<ES[v]+VALOR1_ARCO(adj))
				ES[w]= ES[v] + VALOR1_ARCO(adj);
			GRauE[w]= GRauE[w] - 1; 
			if(GRauE[w] == 0)
				enqueue(w,S);
			adj = PROX_ADJ(adj);
		}
		a = queue_is_Empty(S);
	}
	//print_way(v_f,Prec, DurMin, g);//acabar
}

GRAFO *transposto(GRAFO *g){
	int n = NUM_VERTICES(g);
	GRAFO *gt = new_graph(n);
	int i;
	for(i=0;i<n+1;i++){
		ARCO *adjv = ADJS_NO(i,g);
		while(adjv!=NULL){
			int w = EXTREMO_FINAL(adjv);
			insert_new_arc(w,i,duration, nTrab, gt);
		}
	}
	return gt;
}

void lates_fini(GRAFO*g, int n_Tasks){
	for(i=1;i<=nTasks;i++){
		LF[i]= DurMin;
		GRauS=0;		
	}
	for(i=1;i<=NUM_ARCOS(g);i++){
		for(j=1;j<=NUM_ARCOS(g);j++){
			GRauS[j] ++;
		}
	}
	GRAFO *gt = transposto(g);
	QUEUE* S = mk_empty_queue(nTasks);
	enqueue();
	while(queue_is_Empty!=FALSE){
		int v = dequeue(S);
		ARCO * adjs = ADJS_NO(v,gt);
		while(adjs!=NULL){
			if(LF[w]> LF[v]-VALOR1_ARCO(adjs))
				LF[w]= LF[v] - VALOR1_ARCO(adjs);
			GRauS[w]=GRauS[w]-1;
			if(GRauS[w]==0)
				enqueue(w,S);
			adjs = PROX_ADJ(adjs);
		}
	}
}
int main(){

	GRAFO *g = ler_construir_grafo();//concluida
	earl_star(g, nTasks);
	//lates_fini(g,nTasks);
	printf("Num vertices %d\n", NUM_VERTICES(g));
	printf("Num arcos %d\n", NUM_ARCOS(g));
	return 0;
}