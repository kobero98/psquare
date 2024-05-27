
#include <stdbool.h>

#define AUDIT if(0)

#define SIZE 5

typedef struct psqare{
	float p;
	double marker_heights[SIZE];
	double marker_positions[SIZE];
	double desidered_positions[SIZE];
	double increments[SIZE];
	bool initiated;
	int c;
}psqareEstimator;

void stampaStato(psqareEstimator * p){
	printf("Stato:\n");
	printf("quantile di ordine:%f \n",p->p);
	printf("altezza:\n");
	printf("%f   %f   %f   %f   %f\n",p->marker_heights[0],p->marker_heights[1],p->marker_heights[2],p->marker_heights[3],p->marker_heights[4]);
	printf("posizione:\n");
	printf("%f   %f   %f   %f   %f\n",p->marker_positions[0],p->marker_positions[1],p->marker_positions[2],p->marker_positions[3],p->marker_positions[4]);
	printf("posizione desiderata:\n");
	printf("%f   %f   %f   %f   %f\n",p->desidered_positions[0],p->desidered_positions[1],p->desidered_positions[2],p->desidered_positions[3],p->desidered_positions[4]);
	printf("incremento:\n");
	printf("%f   %f   %f   %f   %f\n",p->increments[0],p->increments[1],p->increments[2],p->increments[3],p->increments[4]);
	printf("flag %d\t c %d \n",p->initiated,p->c);

}

void initPsqaure(psqareEstimator * p,int q){
	p->p= ((float)(q/100.0));
	p->initiated=false;
	p->c=0;
	//init marker heights
	p->marker_heights[0]=0.0;
	p->marker_heights[1]=0.0;
	p->marker_heights[2]=0.0;
	p->marker_heights[3]=0.0;
	p->marker_heights[4]=0.0;
	//init marker position
	p->marker_positions[0]=1.0;
	p->marker_positions[1]=2.0;
	p->marker_positions[2]=3.0;
	p->marker_positions[3]=4.0;
	p->marker_positions[4]=5.0;
	//init desiderated position
	p->desidered_positions[0]=1.0;
	p->desidered_positions[1]=1.0 + 2.0*(p->p);
	p->desidered_positions[2]=1.0 + 4.0*(p->p);
	p->desidered_positions[3]=3.0 + 2.0*(p->p);
	p->desidered_positions[4]=5.0;
	//init increment
	p->increments[0]=0.0;
	p->increments[1]=(p->p)/2.0;
	p->increments[2]=p->p;
	p->increments[3]=(1.0+p->p)/2;
	p->increments[4]=1.0;
	AUDIT printf("Init struct\n");
	AUDIT stampaStato(p);
	return ;
}	

int find_cell(psqareEstimator * p,double newObservation){
    if(newObservation< p->marker_heights[0]){
    	return -1;
    }
    int i=0;
    while( (i+1 < 5) && (newObservation>=p->marker_heights[i+1]) ) i++;
    AUDIT printf("indice cella trovato: %d\n",i);
	return i;
}

double parabolic(psqareEstimator *p,int i, int d){

	double term1,term2,term3;

	term1 = d / (p->marker_positions[i + 1] - p->marker_positions[i - 1]);

    term2 = (p->marker_positions[i] - p->marker_positions[i - 1] + d) * (p->marker_heights[i + 1] - p->marker_heights[i]) / (p->marker_positions[i + 1] - p->marker_positions[i]);

    term3 = (p->marker_positions[i + 1] - p->marker_positions[i] - d) * (p->marker_heights[i] - p->marker_heights[i - 1]) / (p->marker_positions[i] - p->marker_positions[i - 1]);
    AUDIT printf("term1 %f\nterm2 %f\nterm3 %f\n",term1,term2,term3);
    return p->marker_heights[i] + term1 * (term2 + term3); 
}

double linear(psqareEstimator *p,int i,int d){
      return p->marker_heights[i] + d * (p->marker_heights[i + d] - p->marker_heights[i])/ (p->marker_positions[i+d] - p->marker_positions[i]);
}

void adjust_height_values(psqareEstimator *p){
    int i;
    float d;
    int d2;

    for(i=1;i<SIZE-1;i++){
    	d= p->desidered_positions[i] - p->marker_positions[i];
    	AUDIT printf("i %d d %f \n",i,d);
    	if(d>=1.0 && (p->marker_positions[i+1]-p->marker_positions[i]>1) || (d<=-1.0 && (p->marker_positions[i-1] - p->marker_positions[i] ) < -1   ) ){
    		if(d<0){ d2=-1;}
    		else{ d2=1;}
    		double qprime;
    		qprime =parabolic(p,i,d2);
    		AUDIT printf("parabolic qprime: %f\n",qprime);
    		if(p->marker_heights[i-1] < qprime && qprime < p->marker_heights[i+1]){
    			p->marker_heights[i]=qprime;

    		}else{
    			qprime = linear(p,i,d2);
    			AUDIT printf("linear qprime: %f\n",qprime);
    			p->marker_heights[i]=qprime;
    		}

    		p->marker_positions[i] += d2;
    	}
    }
}

void update(psqareEstimator *p,long long new_observation){
	float app;
	int i,j,k;
	double newObservation;

	newObservation= (double) new_observation;
	AUDIT printf("\n\nvalore nuova osservazione: %f %lld \n",newObservation,new_observation);
	if(p->initiated==false){
		p->marker_heights[p->c]=newObservation;
		p->c=p->c+1;
		if(p->c==5){
			for(i=0;i<SIZE;i++){
				for(j=i;j<SIZE;j++){
					if(p->marker_heights[i]>p->marker_heights[j]){
						app=p->marker_heights[i];
						p->marker_heights[i]=p->marker_heights[j];
						p->marker_heights[j]=app;
					}
				}

			}
			p->initiated=true;
			AUDIT printf("\n\ndopo 5 elementi\n");
		}
		AUDIT stampaStato(p);
		return;
	}
	
    AUDIT printf("\nCerco la Cella\n");
    i=find_cell(p,newObservation);
    if(i==-1){
    	p->marker_heights[0] = newObservation;
    	k=0;
    }else{
	    if(i==4){
	    	p->marker_heights[4]= newObservation;
	    	k=3;
	    }else{
	    	k=i;
	    }
    }
    AUDIT stampaStato(p);
    
    AUDIT printf("\nAggiorno la posizione\n");
    for(j=k+1;j<SIZE;j++){
        	p->marker_positions[j]++;
    }
    AUDIT stampaStato(p);
    AUDIT printf("\nAggiorno la posizione desiderata\n");
    for(j=0;j<SIZE;j++){
    	p->desidered_positions[j]=p->desidered_positions[j]+p->increments[j];

    }
    AUDIT stampaStato(p);
    AUDIT printf("\nfaccio l'aggiustamento dell'altezza\n");
    adjust_height_values(p);
	AUDIT stampaStato(p);
	return;
}

long long p_estimate(psqareEstimator *p){
	if( p->c > 2){
	    return (long long)p->marker_heights[2];
	}
    return 0; //dovrei sostituirlo con qualcos'altro
}


