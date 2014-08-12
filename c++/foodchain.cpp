#include "includes.h"

#define GENELEN 2048
#define MUTATION 1e-3
#define GENEDUP 1e-3
#define LETTERS 20
#define ACTIVE 2
#define REAPER 1e-3

int XR,YR;

class Organism
{
	public:
		char Genome[GENELEN];
		char alive;
		int age;
		int maxatk,maxdef;
		
		void Replicate(Organism *Target);
		int canEat(Organism *Target);
		vector<string> getAttackStrings();
		vector<string> getDefenseStrings();
};

Organism *Grid;

int prand(double prob)
{
	if (rand()%1000001<1000000*prob) return 1;
	return 0;
}

void Organism::Replicate(Organism *Target)
{
	int i;
	
	Target->age = 0;
	Target->alive = 1;
	
	for (i=0;i<GENELEN;i++)
	{
		Target->Genome[i]=Genome[i];
		
		if (prand(MUTATION))
		{
			Target->Genome[i]=rand()%(2*LETTERS);
		}
	}
	
	if (prand(GENEDUP))
	{
		int start=rand()%GENELEN, end=rand()%GENELEN, newstart=rand()%GENELEN;
		int buf=0;
		
		if (start>end)
		{
			buf=end; end=start; start=buf;
		}
		
		for (i=newstart;(i<newstart+(end-start))&&(i<GENELEN);i++)
		{
			Genome[i]=Genome[i-newstart+start];
		}
	}
}

int conjugate(int base)
{
	if (base<LETTERS) return base+LETTERS;
	else return base-LETTERS;
}

int isAttack(int base)
{
	if (base<ACTIVE) return 1;
	return 0;
}

int isDefense(int base)
{
	if ((base>=LETTERS)&&(base<ACTIVE+LETTERS)) return 1;
	return 0;
}

vector<string> Organism::getAttackStrings()
{
	vector<string> list;
	int i,j,k;
	
	for (i=0;i<GENELEN;i++)
	{
		if (isAttack(Genome[i]))
		{
			int stop = 0;
			string S = "";
			
			j=0;
			do
			{
				if (!isAttack(Genome[i+j]))
					stop=1;
				else
					S = S + (char)(Genome[i+j]+'A');
					
				j++;
			} while ((!stop)&&(i+j<GENELEN));
			
			list.push_back(S);
			i=i+j-1;
		}
	}			
	
	return list;
}

vector<string> Organism::getDefenseStrings()
{
	vector<string> list;
	int i,j,k;

	for (i=0;i<GENELEN;i++)
	{
		if (isDefense(Genome[i]))
		{
			int stop = 0;
			string S = "";
			
			j=0;
			do
			{
				if (!isDefense(Genome[i+j]))
					stop=1;
				else
					S = S + (char)(Genome[i+j]+'A');
					
				j++;
			} while ((!stop)&&(i+j<GENELEN));
			
			list.push_back(S);
			i=i+j-1;
		}
	}			
	
	return list;
}

int Organism::canEat(Organism *Target)
{
	int i,j,k,l;
	int mc = 0;
	
	for (i=0;i<GENELEN;i++)
	{
		if (isAttack(Genome[i]))
		{
			int stop = 0;
			
			j=0;
			do
			{				
				if (!isAttack(Genome[i+j]))
					stop=1;
				j++;
			} while ((!stop)&&(i+j<GENELEN));
			
			int found = 0;
			
			for (k=0;k<GENELEN;k++)
			{
				stop = 0; l = 0;
				do
				{
					if (Target->Genome[k+l] != conjugate(Genome[i+l]))
						stop = 1;	
					l++;
				} while ((!stop)&&(k+l<GENELEN)&&(l<j));
				
				if ((k+l!=GENELEN)&&(!stop))
				{
					found = 1;
				}
			}
			
			if (!found) { return 1; }
			else mc++;
			i=i+j-1;
		}		
	}
	
//	printf("Survived!\n");
	return 0;
}

void Iterate()
{
	int step=0;
	int x,y,xm,ym;
	int substep;
	
	for (step=0;step<XR*YR;step++)
	{
		x=rand()%XR; y=rand()%YR;
		
		if (Grid[x+y*XR].alive)
		{
			Grid[x+y*XR].age++;
			if (prand(REAPER))
			{
				Grid[x+y*XR].alive = 0;
			}
			else
			{
				for (substep=0;substep<3;substep++)
				{	
					xm=x+rand()%3-1; ym=y+rand()%3-1;
				
					if ((xm!=x)||(ym!=y))
					{
						if (xm>=XR) xm-=XR; if (xm<0) xm+=XR;
						if (ym>=YR) ym-=YR; if (ym<0) ym+=YR;
					
						if (!Grid[xm+ym*XR].alive)
						{
							Grid[x+y*XR].Replicate(&Grid[xm+ym*XR]);					
						}
						else
						{
							if (Grid[x+y*XR].canEat(&Grid[xm+ym*XR]))
							{
								Grid[x+y*XR].Replicate(&Grid[xm+ym*XR]);
							}
							else
							{
								Organism Buf = Grid[x+y*XR];
								Grid[x+y*XR]=Grid[xm+ym*XR];
								Grid[xm+ym*XR] = Buf;
								
								x=xm; y=ym;
							}
						}
					}			
				}
			}		
		}
	}
}

unordered_map<string, int> stringList;

int curX=0;

void displayPackardPlot()
{
	int y,r,g,b;
	unordered_map<string,int>::iterator it;
	
	curX++;
	
	if (curX>=XRes-XR)
	{
		int x;
		
		for (x=0;x<XRes-1-XR;x++)
		{
			for (y=0;y<YRes;y++)
			{
				ScreenBuf[(x+y*XRes)*Bpp] = ScreenBuf[(x+1+y*XRes)*Bpp];
				ScreenBuf[(x+y*XRes)*Bpp+1] = ScreenBuf[(x+1+y*XRes)*Bpp+1];
				ScreenBuf[(x+y*XRes)*Bpp+2] = ScreenBuf[(x+1+y*XRes)*Bpp+2];
			}
		}
		x=XRes-1;
		for (y=0;y<YRes;y++)
		{
			ScreenBuf[(x+y*XRes)*Bpp] = 0;
			ScreenBuf[(x+y*XRes)*Bpp+1] = 0;
			ScreenBuf[(x+y*XRes)*Bpp+2] = 0;
		}
		curX--;
	}	
	
	for (it=stringList.begin();it!=stringList.end();++it)
	{
		y=YRes-it->second/10000;
		
		if ((y>=0)&&(y<YRes))
		{
			b=ScreenBuf[(curX+y*XRes)*Bpp];
			g=ScreenBuf[(curX+y*XRes)*Bpp+1];
			r=ScreenBuf[(curX+y*XRes)*Bpp+2];
			
			if (it->first[0]-'A'<LETTERS)
				b+=64;
			else r+=64; //r+=64; g+=64; b+=64; 
			
			
			if (r>255) r=255; if (g>255) g=255; if (b>255) b=255;
			
			ScreenBuf[(curX+y*XRes)*Bpp]=b;
			ScreenBuf[(curX+y*XRes)*Bpp+1]=g;
			ScreenBuf[(curX+y*XRes)*Bpp+2]=r;
		}
	}		
}

void Render()
{
	int x,y,xm,ym;
	
	for (y=0;y<YR;y++)
		for (x=0;x<XR;x++)
		{
			xm=x+800; ym=y;
			int r,g,b;
			
			if (Grid[x+y*XR].alive)
			{
				r=Grid[x+y*XR].age; if (r>255) r=255;
				g=Grid[x+y*XR].maxatk*10; if (g>255) g=255;
				b=Grid[x+y*XR].maxdef*10; if (b>255) b=255;
			}
			else
			{
				r=g=b=0;
			}
			
			ScreenBuf[(xm+ym*XRes)*Bpp]=b;
			ScreenBuf[(xm+ym*XRes)*Bpp+1]=g;
			ScreenBuf[(xm+ym*XRes)*Bpp+2]=r;
		}
		
	BlitBuf(ScreenBuf,0,0,XRes,YRes);
}

void Survey(double *avgatk, double *avgdef)
{
	int i,j;
	int maxAtk=0, maxDef=0;
	int pop=0;
	
	(*avgatk) = 0; (*avgdef) = 0;
	
	for (i=0;i<XR*YR;i++)
	{
		if (Grid[i].alive)
		{
			pop++;
			
			vector<string> aList = Grid[i].getAttackStrings();
			vector<string> dList = Grid[i].getDefenseStrings();
			
			maxAtk = 0; maxDef = 0;
			for (j=0;j<aList.size();j++)
			{
				if (aList[j].length() > maxAtk) maxAtk = aList[j].length();
				
				stringList[aList[j]] += 1;
			}
			
			for (j=0;j<dList.size();j++)
			{
				if (dList[j].length() > maxDef) maxDef = dList[j].length();

				stringList[dList[j]] += 1;
			}
			
			Grid[i].maxatk = maxAtk; Grid[i].maxdef = maxDef;
			(*avgatk) += maxAtk; (*avgdef) += maxDef;
		}
	}
	
	displayPackardPlot();
	Render();
	
	(*avgatk) = (*avgatk)/pop;
	(*avgdef) = (*avgdef)/pop;
}

void InitSystem()
{
	int x,y,i;
	
	XR=128; YR=128;
	
	Grid=(Organism*)malloc(XR*YR*sizeof(Organism));
		
	for (y=0;y<YR;y++)
	{
		for (x=0;x<XR;x++)
		{
			if (rand()%4==0)
			{
				Grid[x+y*XR].alive = 1;
				Grid[x+y*XR].age = 0;
				
				for (i=0;i<GENELEN;i++)
				{
					do
					{
						Grid[x+y*XR].Genome[i]=rand()%(2*LETTERS);
					} while ( (Grid[x+y*XR].Genome[i]<ACTIVE) || ((Grid[x+y*XR].Genome[i]>=LETTERS)&&(Grid[x+y*XR].Genome[i]<LETTERS+ACTIVE)) );
				}
			}
			else
			{
				Grid[x+y*XR].alive = 0;
				Grid[x+y*XR].age = 0;
			}
		}
	}
}

int main(int argc, char **argv)
{
	FILE *f;
	double atk,def;
	int t=0;
	
	InitSystem();
	
	Bpp=4;
	XRes=800+XR; YRes=400; 

	ScreenBuf=(unsigned char*)malloc(XRes*YRes*Bpp);
	memset(ScreenBuf,0,XRes*YRes*Bpp);
	InitSDL();	
	
	while (1)
	{
		int Ch=ReadKey();
		
		if (Ch=='q') exit(0);
		Iterate();
		Survey(&atk,&def);
		
		f=fopen("output.txt","a");
		fprintf(f,"%d %.6g %.6g\n",t,atk,def);
		fclose(f);
		t++;
	}
}
