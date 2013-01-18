/*
 * I was really fucking bored and depressed...
 */

#include <stdint.h>
#include "TendencyMatrix10x25.h"

#define ZERO_FITNESS_LIMIT			1.0e-5
__float128 qPriorLikelihood[(1<<NUMBER_DISEASES)] = {};

__float128 fitness(uint32_t diagnosis, uint32_t symptoms)
{
	__float128 L1 = 1.0;
	__float128 L2 = 1.0;
	uint32_t iter, jter;

// Positive Likelihood
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		if (symptoms&(1<<iter))
		{
			__float128 temp = 1.0;
			for (jter=0; jter<NUMBER_DISEASES; jter++)
			{
				if (diagnosis&(1<<jter))
				{
					temp *= (1-qManifestationInDisease[iter][jter]);
				}
			}
			__float128 temp2 = 1.0;
			L1 *= (temp2-temp);
		}
	}

// Negative Likelihood
	for (iter=0; iter<NUMBER_DISEASES; iter++)
	{
		if (diagnosis&(1<<iter))
		{
			__float128 temp = 1.0;
			for (jter=0; jter<NUMBER_SYMPTOMS; jter++)
			{
				if (!(symptoms&(1<<jter)))
				{
					__float128 temp2 = 1.0;
					temp *= (temp2-qManifestationInDisease[jter][iter]);
				}
			}
			L2 *= (temp);
		}
	}
	return (L1 * L2 * qPriorLikelihood[diagnosis]);
}

void priorLikelihoodSetup(void)
{
	uint32_t iter, jter;
	for (iter=0; iter<(1<<NUMBER_DISEASES); iter++)
	{
		qPriorLikelihood[iter] = 1.0;
		for (jter=0; jter<NUMBER_DISEASES; jter++)
		{
			if (iter&(1<<jter))
			{
				qPriorLikelihood[iter] *= (qPriorProbability[jter])/(1-qPriorProbability[jter]);
			}
		}
	}
}

void tendencyFix(void)
{
	uint32_t iter, jter;
	for (iter=0; iter<NUMBER_SYMPTOMS; iter++)
	{
		for (jter=0; jter<NUMBER_DISEASES; jter++)
		{
			if (qManifestationInDisease[iter][jter] < ZERO_FITNESS_LIMIT)
			{
				qManifestationInDisease[iter][jter] = ZERO_FITNESS_LIMIT;
			}
		}
	}
}

