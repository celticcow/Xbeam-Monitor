#include "cpu.h"

//constructors
cpu::cpu()
{
	cpu_user = 0;
	cpu_nice = 0;
	cpu_system = 0;
	cpu_idle = 0;
	cpu_iowait = 0;
	cpu_irq = 0;
	cpu_softirq = 0;
}

cpu::cpu(unsigned long long int parts[7])
{
	cpu_user = parts[0];
	cpu_nice = parts[1];
	cpu_system = parts[2];
	cpu_idle = parts[3];
	cpu_iowait = parts[4];
	cpu_irq = parts[5];
	cpu_softirq = parts[6];
}

void cpu::set_user(unsigned long long int data)
{
	cpu_user = data;
}

void cpu::set_nice(unsigned long long int data)
{
	cpu_nice = data;
}

void cpu::set_system(unsigned long long int data)
{
	cpu_system = data;
}

void cpu::set_idle(unsigned long long int data)
{
	cpu_idle = data;
}

void cpu::set_iowait(unsigned long long int data)
{
	cpu_iowait = data;
}

void cpu::set_irq(unsigned long long int data)
{
	cpu_irq = data;
}

void cpu::set_softirq(unsigned long long int data)
{
	cpu_softirq = data;
}

/*unsigned long long int[7] cpu::cpu_allcpu()
{
	unsigned long long int all[7];
	all[0] = cpu_user;
	all[1] = cpu_nice;
	all[2] = cpu_system;
	all[3] = cpu_idle;
	all[4] = cpu_iowait;
	all[5] = cpu_irq;
	all[6] = cpu_softirq;
	
	return(all);
}*/

unsigned long long int cpu::get_user()
{
	return(cpu_user);
}

unsigned long long int cpu::get_nice()
{
	return(cpu_nice);
}

unsigned long long int cpu::get_system()
{
	return(cpu_system);
}

unsigned long long int cpu::get_idle()
{
	return(cpu_idle);
}

unsigned long long int cpu::get_iowait()
{
	return(cpu_iowait);
}

unsigned long long int cpu::get_irq()
{
	return(cpu_irq);
}

unsigned long long int cpu::get_softirq()
{
	return(cpu_softirq);
}
