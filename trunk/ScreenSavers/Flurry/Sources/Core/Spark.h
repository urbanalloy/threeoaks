// Spark.h: interface for the Spark class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SPARK_H_
#define __SPARK_H_


typedef struct Spark  
{
    float position[3];
    int mystery;
    float delta[3];
    float color[4];    
} Spark;

__private_extern__ void InitSpark(Spark *s);
__private_extern__ void UpdateSpark(Spark *s);
__private_extern__ void UpdateSparkColour(Spark *s);


#endif // __SPARK_H_
