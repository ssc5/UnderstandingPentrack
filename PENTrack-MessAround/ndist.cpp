#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>

int neutdist = 0;
std::vector<std::vector<double> > ndist;
int n_r = 350, n_z = 1000;	// number of bins
double size = 0.002, rmin = 0, zmin = -0.8;	// size of bins, position of first bin


// prepare neutron density histogram matrix
void prepndist()
{
	printf("Reserving space for neutron distribution matrix (%f MB)\n",(float)(n_r*n_z*sizeof(double))/1024/1024);
	ndist.resize(n_r);
	for (int i = 0; i < n_r; i++)
		ndist[i].resize(n_z);
}

/// fill neutron density histogram
void fillndist(double x1, double y1[6], double x2, double y2[6])
{
	double r1 = sqrt(y1[0]*y1[0] + y1[1]*y1[1]);
	double z1 = y1[2];
	double r2 = sqrt(y2[0]*y2[0] + y2[1]*y2[1]);
	double z2 = y2[2];
	int ir1 = int((r1 - rmin)/size);
	int iz1 = int((y1[2] - zmin)/size);
	int ir2 = int((r2 - rmin)/size);
	int iz2 = int((y2[2] - zmin)/size);
	while (ir1 != ir2 || iz1 != iz2){
		double s_left = (ir1*size + rmin - r1)/(r2 - r1);
		double s_right = ((ir1+1)*size + rmin - r1)/(r2 - r1);
		double s_bottom = (iz1*size + zmin - z1)/(z2 - z1);
		double s_top = ((iz1+1)*size + zmin - z1)/(z2 - z1);
		double s;
		int newir = ir1, newiz = iz1;
		if (s_left > 0){
			if (s_bottom > 0){
				if (s_left > s_bottom) newiz = iz1-1;
				else newir = ir1-1;
				s = std::min(s_left, s_bottom);
			}
			else{
				if (s_left > s_top) newiz = iz1+1;
				else newir = ir1-1;
				s = std::min(s_left, s_top);
			}
		}
		else{
			if (s_bottom > 0){
				if (s_right > s_bottom) newiz = iz1-1;
				else newir = ir1+1;
				s = std::min(s_right, s_bottom);
			}
			else{
				if (s_right > s_top) newiz = iz1+1;
				else newir = ir1+1;
				s = std::min(s_right, s_top);
			}
		}
		if(ir1 >= n_r || iz1 >= n_z)
			printf("Ndist Error %d %d not in %d %d\n",ir1,iz1,n_r,n_z);
		else
			ndist[ir1][iz1] += s*(x2 - x1);
		ir1 = newir;
		iz1 = newiz;
		r1 += (r2 - r1)*s;
		z1 += (z2 - z1)*s;
		x1 += (x2 - x1)*s;
	}
	if(ir2 >= n_r || iz2 >= n_z)
		printf("Ndist Error %d %d not in %d %d\n",ir2,iz2,n_r,n_z);
	else
		ndist[ir2][iz2] += x2 - x1;
}

// print neutron density distribution in ndist.out
void outndist(const char *ndistfile)
{
	//printf("\nOutputting the particle spacial distribution... \n");

	FILE *NDIST=fopen(ndistfile,"w");
	int Treffer = 0;

	//FILE *NDIST=fopen("ndist.out",mode_w);

	fprintf(NDIST,"Rindex Zindex Rmtlpkt Zmtlpkt Whk Treffer\n");
	for(int i=0; i<n_r; i++){
		for(int j=0; j<n_z; j++){
			double val = ndist[i][j];
			if (val != 0) Treffer =1;
			if (val == 0) Treffer =0;
			fprintf(NDIST,"%i %i %.5G %.5G %.17G %i\n",i,j, i*size + rmin + size/2, j*size + zmin + size/2, val, Treffer);
		}
	}
	fclose(NDIST);
	ndist.clear();
}
