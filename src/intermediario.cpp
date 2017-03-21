#include "ros/ros.h"
#include "sensor_msgs/Joy.h"                                            //per leggere i dati dal joystick
#include "geometry_msgs/Twist.h"                                        //per inviare i dati a cmd_vel per il robot
#include <pthread.h>
#include <errno.h>
typedef struct comandi_s{
	float x;
	float z;
}comandi_t;

comandi_t* cmd;


ros::Publisher pilota;                                                  // dichiaro la variabile pilota globalmente in modo che la possa leggere anche cback


void *Worker_thread(void* com_params ){
	comandi_t* c=(comandi_t*) com_params;
	geometry_msgs::Twist mex;                                           //il tread crea il messaggio da inviare

	while(ros::ok()){
		mex.linear.x=c->x;                                              //prepara il messaggio (1)
		mex.angular.z=c->z;                                             //prepara il messaggio (2)
		pilota.publish(mex);                                            //invia una volta i comandi
		ros::Rate loop_rate(20);
		while((mex.linear.x==c->x)&&(mex.angular.z==c->z)){             //se non arrivano altri comandi, invia 20 volte al secondo lo stesso comando
			pilota.publish(mex);                                        //invia ripetutamente i dati al topic cmd_vel 
			loop_rate.sleep();
			}                                                           //se i comandi cambiano, esce dal while più interno e ricomincia quello esterno
		}                                                               //se ros::ok()==false esce dal while esterno, libera la memoria e termina
	free(c);
	//free(mex);
	return NULL;
	}



void cback(const sensor_msgs::Joy::ConstPtr& msg){
	cmd->x=msg->axes[1];
	cmd->z=msg->axes[0];	
	//float volante= msg->axes[0];                                        //leggo di quanto deve curvare
	//float vel= msg->axes[1];                                            //leggo a che velocità e verso deve andare
	
	//geometry_msgs::Twist mex;                                           //creo il messaggio da inviare:
	//mex.linear.x=vel;                                                   //linear.x indica la velocità e il verso in twist mex
	//mex.angular.z=volante;                                              //angular.z indica di quanto deve girare
    
   // ros::Rate loop_rate(20);
	//while(true){
	//	pilota.publish(mex);                                              //invio i dati al topic joy
	//	loop_rate.sleep();
	//	}
	}
	
int main(int argc, char **argv){
	ros::init(argc,argv,"intermediario");                               //inizializzo il nodo con il nome di intermediario, cioè quello che prende i dati
																		//dal joystick e li manda a cmd_vel, il topic al quale è iscritto stageros
	
	//ROS_INFO("Controllo ripetizioni");
	cmd=(comandi_t*)malloc(sizeof(comandi_t));
	cmd->x=0.0;
	cmd->z=0.0;
	ros::NodeHandle n;
	pilota=n.advertise<geometry_msgs::Twist>("cmd_vel",1000);  
	pthread_t worker;
	if(pthread_create(&worker,NULL,Worker_thread,cmd)){
		ROS_INFO("Errore nella creazione del thread: %s",strerror(errno));
		exit(1);
		}
	         //setto a cmd_vel il topic in cui voglio pubblicare
	ros::Subscriber sub=n.subscribe("joy",1000,cback);                  //iscrivo il nodo appena creato al topic del joystick "joy"
	ros::spin();
	return 0;
	}
