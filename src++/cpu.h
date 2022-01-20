#ifndef _CPU_H
#define _CPU_H

/**
 * CPU class needed to create linked list of cpu cores
 * 0 user
 * 1 nice
 * 2 system
 * 3 idle
 * 4 iowait (think)
 * 5 irq (think)
 * 6 soft irq 
 * 
 **/

class cpu
{
	public:
	//Constructors
	cpu();
	cpu(unsigned long long int parts[7]);
	
	//Modifiers
	void set_user(unsigned long long int data);
	void set_nice(unsigned long long int data);
	void set_system(unsigned long long int data);
	void set_idle(unsigned long long int data);
	void set_iowait(unsigned long long int data);
	void set_irq(unsigned long long int data);
	void set_softirq(unsigned long long int data);
	
	//Accessors
	//unsigned long long int[7] get_allcpu();
	unsigned long long int get_user();
	unsigned long long int get_nice();
	unsigned long long int get_system();
	unsigned long long int get_idle();
	unsigned long long int get_iowait();
	unsigned long long int get_irq();
	unsigned long long int get_softirq();
	
	
	private:
	//cpu core info
	unsigned long long int cpu_user;
	unsigned long long int cpu_nice;
	unsigned long long int cpu_system;
	unsigned long long int cpu_idle;
	unsigned long long int cpu_iowait;
	unsigned long long int cpu_irq;
	unsigned long long int cpu_softirq;
};

#endif
