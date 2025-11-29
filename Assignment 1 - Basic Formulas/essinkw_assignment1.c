#include <stdio.h>
#include <math.h>

//constants
const double PI = 3.14159265359;

//function definitions
int getNumSpheres(void);
double findSphereSA(double R, double ha, double hb);
double findSphereV(double R, double ha, double hb);
double findA(double R, double ha);
double findB(double R, double hb);
double findH(double ha, double hb);

int main(void) {

//variables
int numSpheres;
double R;
double ha;
double hb;
double SA = 0.0;
double V = 0.0;
double SAAverage = 0.0;
double VAverage = 0.0;

numSpheres = getNumSpheres();

for(int i = 0; i < numSpheres; i++)
{
  do
  {
    printf("Obtaining data for spherical segment number %d\n", i);

    //get R
    printf("What is the radius of the sphere (R)?\n");
    scanf("%lf", &R);

    //get ha
    printf("What is the height of the top area of the spherical segment (ha)?\n");
    scanf("%lf", &ha);

    //get hb
    printf("What is the height of the bottom area of the spherical segment (hb)?\n");
    scanf("%lf", &hb);

    printf("Entered data: R = %.2lf ha = %.2lf hb = %.2lf.\n", R, ha, hb);

    //error check
    if(R < 0 || ha < 0 || hb < 0 || R < ha || R < hb || ha < hb)
    {
      printf("Invalid Input.\n");
    }
  } while(R < 0 || ha < 0 || hb < 0 || R < ha || R < hb || ha < hb);

  //find surface area and volume
  SA = findSphereSA(R, ha, hb);
  V = findSphereV(R, ha, hb);

  printf("Total Surface Area = %.2lf Volume = %.2lf.\n", SA, V);

  //store SA and V values to find the averages later
  SAAverage += SA;
  VAverage += V;
}

//find surface area and volume averages
SAAverage = SAAverage / numSpheres;
VAverage = VAverage / numSpheres;

//print averages
printf("Average Surface Area = %.2lf Average Volume = %.2lf.\n", SAAverage, VAverage);

return 0;
}

//gets the number of spheres the user wants to perform calculations on
int getNumSpheres(void)
{
  int numSpheres;

  //loops until input is valid
  while(numSpheres < 2 || numSpheres > 10)
  {
    printf("How many spherical segments you want to evaluate [2-10]?\n");
    scanf("%d", &numSpheres);
  }

  return numSpheres;
}

//does the math to find the sphere's surface area 
double findSphereSA(double R, double ha, double hb)
{
  double a;
  double b;
  double h;
  double TA;
  double BA;
  double LA;
  double SA;

  //find a, b, and c
  a = findA(R, ha);
  b = findB(R, hb);
  h = findH(ha, hb);

  //find top. bottom, and lateral surface area
  TA = PI * (a * a);
  BA = PI * (b * b);
  LA = 2 * PI * R * h;

  //calculate total surface area
  SA = TA + BA + LA;
    
  return SA;
}

//does the math to find the sphere's volume
double findSphereV(double R, double ha, double hb)
{
  double a;
  double b;
  double h;
  double V;
  
  //find a, b, and c
  a = findA(R, ha);
  b = findB(R, hb);
  h = findH(ha, hb);

  //calculate volume
  V = (1.0/6.0) * PI * h * (3.0 * (a * a) + 3.0 * (b * b) + (h * h));

  return V;
}

//finds a
double findA(double R, double ha)
{
  double a;

  a = sqrt((R * R) - (ha * ha));
    
  return a;
}

//finds b
double findB(double R, double hb)
{
  double b;

  b = sqrt((R * R) - (hb * hb));
    
  return b;
}

//finds h
double findH(double ha, double hb)
{
  double h;

  h = ha - hb;
    
  return h;
}
