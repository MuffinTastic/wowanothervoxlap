#ifndef KEN_VOXLAP5_H
#define KEN_VOXLAP5_H

#define MAXXDIM 1920
#define MAXYDIM 1080
#define PI 3.141592653589793
#define VSID 512   //Maximum .VXL dimensions in both x & y direction
#define MAXZDIM 64 //Maximum .VXL dimensions in z direction (height)

#define DLLEXPORT //__declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push,1)

		typedef struct kv6data kv6data;

		typedef struct lpoint3d { long x, y, z; } lpoint3d;
		typedef struct point3d { float x, y, z; } point3d;
		typedef struct point4d { float x, y, z, z2; } point4d;
		typedef struct dpoint3d { double x, y, z; } dpoint3d;

		//Sprite structures:
		typedef struct kv6voxtype { long col; unsigned short z; char vis, dir; } kv6voxtype;

		typedef struct kv6data
		{
			long leng, xsiz, ysiz, zsiz;
			float xpiv, ypiv, zpiv;
			unsigned long numvoxs;
			long namoff;
			kv6data *lowermip;
			kv6voxtype *vox;      //numvoxs*sizeof(kv6voxtype)
			unsigned long *xlen;  //xsiz*sizeof(long)
			unsigned short *ylen; //xsiz*ysiz*sizeof(short)
		} kv6data;

		typedef struct hingetype
		{
			long parent;      //index to parent sprite (-1=none)
			point3d p[2];     //"velcro" point of each object
			point3d v[2];     //axis of rotation for each object
			short vmin, vmax; //min value / max value
			char htype, filler[7];
		} hingetype;

		typedef struct seqtyp { long tim, frm; } seqtyp;

		typedef struct kfatype
		{
			long numspr, numhin, numfrm, seqnum;
			long namoff;
			kv6data *basekv6;      //Points to original unconnected KV6 (maybe helpful?)
			struct vx5sprite *spr; //[numspr]
			hingetype *hinge;      //[numhin]
			long *hingesort;       //[numhin]
			short *frmval;         //[numfrm][numhin]
			seqtyp *seq;           //[seqnum]
		} kfatype;

		//Notice that I aligned each point3d on a 16-byte boundary. This will be
		//   helpful when I get around to implementing SSE instructions someday...
		typedef struct vx5sprite
		{
			point3d p; //position in VXL coordinates
			long flags; //flags bit 0:0=use normal shading, 1=disable normal shading
			//flags bit 1:0=points to kv6data, 1=points to kfatype
			//flags bit 2:0=normal, 1=invisible sprite
			union { point3d s, x; }; //kv6data.xsiz direction in VXL coordinates
			union
			{
				kv6data *voxnum; //pointer to KV6 voxel data (bit 1 of flags = 0)
				kfatype *kfaptr; //pointer to KFA animation  (bit 1 of flags = 1)
			};
			union { point3d h, y; }; //kv6data.ysiz direction in VXL coordinates
			long kfatim;        //time (in milliseconds) of KFA animation
			union { point3d f, z; }; //kv6data.zsiz direction in VXL coordinates
			long okfatim;       //make vx5sprite exactly 64 bytes :)
		} vx5sprite;

		//Falling voxels shared data: (flst = float list)
#define FLPIECES 256 //Max # of separate falling pieces
		typedef struct flstboxtype//(68 bytes)
		{
			lpoint3d chk; //a solid point on piece (x,y,pointer) (don't touch!)
			long i0, i1; //indices to start&end of slab list (don't touch!)
			long x0, y0, z0, x1, y1, z1; //bounding box, written by startfalls
			long mass; //mass of piece, written by startfalls (1 unit per voxel)
			point3d centroid; //centroid of piece, written by startfalls

			//userval is set to -1 when a new piece is spawned. Voxlap does not
			//read or write these values after that point. You should use these to
			//play an initial sound and track velocity
			long userval, userval2;
		} flstboxtype;

		//Lighting variables: (used by updatelighting)
#define MAXLIGHTS 256
		typedef struct lightsrctype { point3d p; float r2, sc; } lightsrctype;

		//Used by setspans/meltspans. Ordered this way to allow sorting as longs!
		typedef struct vspans { char z1, z0, x, y; } vspans;

#pragma pack(pop)

#define MAXFRM 1024 //MUST be even number for alignment!

		//Voxlap5 shared global variables:
#ifndef VOXLAP5
		extern
#endif
		struct vx5_struct
		{
			//------------------------ DATA coming from VOXLAP5 ------------------------

			//Clipmove hit point info (use this after calling clipmove):
			double clipmaxcr; //clipmove always calls findmaxcr even with no movement
			dpoint3d cliphit[3];
			long cliphitnum;

			//Bounding box written by last set* VXL writing call
			long minx, miny, minz, maxx, maxy, maxz;

			//Falling voxels shared data:
			long flstnum;
			flstboxtype flstcnt[FLPIECES];

			//Total count of solid voxels in .VXL map (included unexposed voxels)
			long globalmass;

			//Temp workspace for KFA animation (hinge angles)
			//Animsprite writes these values&you may modify them before drawsprite
			short kfaval[MAXFRM];

			//------------------------ DATA provided to VOXLAP5 ------------------------

			//Opticast variables:
			long anginc;
			long sideshademode, mipscandist, maxscandist, vxlmipuse, fogcol;

			//Drawsprite variables:
			long kv6mipfactor, kv6col;
			//Drawsprite x-plane clipping (reset to 0,(high int) after use!)
			//For example min=8,max=12 permits only planes 8,9,10,11 to draw
			long xplanemin, xplanemax;

			//Map modification function data:
			long curcol, currad, curhei;
			float curpow;

			//Procedural texture function data:
			long(*colfunc)(lpoint3d *);
			long cen, amount, *pic, bpl, xsiz, ysiz, xoru, xorv, picmode;
			point3d fpico, fpicu, fpicv, fpicw;
			lpoint3d pico, picu, picv;
			float daf;

			//Lighting variables: (used by updatelighting)
			long lightmode; //0 (default), 1:simple lighting, 2:lightsrc lighting
			lightsrctype lightsrc[MAXLIGHTS]; //(?,?,?),128*128,262144
			long numlights;

			long fallcheck;
		} vx5;

		//Initialization functions:
		long initvoxlap();
		void uninitvoxlap();

		//File related functions:
		long loadsxl(const char *, char **, char **, char **);
		char *parspr(vx5sprite *, char **);
		void loadnul(point3d *, point3d *, point3d *, point3d *);
		long loaddta(const char *, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		long loadpng(const char *, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		void loadbsp(const char *, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		long loadvxl(const char *); //, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		long savevxl(const char *); //, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		long loadvxl_new(const char *, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		long savevxl_new(const char *, dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *);
		long loadsky(const char *);

		// also this -muffin
		long loadvxlbytes(const char *, long);

		//Screen related functions:
		void voxsetframebuffer(long, long, long, long);
		void setsideshades(char, char, char, char, char, char);
		void setcamera_float(point3d *, point3d *, point3d *, point3d *, float, float, float); // for less casting. casting is a pain with vectors! -muffin
		void setcamera(dpoint3d *, dpoint3d *, dpoint3d *, dpoint3d *, float, float, float);
		void opticast();
		void drawpoint2d(long, long, long);
		void drawpoint3d(float, float, float, long);
		void drawline2d(float, float, float, float, long);
		void drawline3d(float, float, float, float, float, float, long);
		long project2d(float, float, float, float *, float *, float *);
		void drawspherefill(float, float, float, float, long);
		void drawsquarefill(float, float, float, float, long);
		void drawpicinquad(long, long, long, long, long, long, long, long, float, float, float, float, float, float, float, float);
		void drawpolyquad(long, long, long, long, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float);
		void print4x6(long, long, long, long, const char *, ...);
		void print6x8(long, long, long, long, const char *, ...);
		void drawtile(long, long, long, long, long, long, long, long, long, long, long, long);
		long screencapture32bit(const char *);
		long surroundcapture32bit(dpoint3d *, const char *, long);

		//Sprite related functions:
		kv6data *loadkv6(const char *);
		kv6data *getkv6(const char *);
		kfatype *getkfa(const char *);
		void freekv6(kv6data *kv6);
		void savekv6(const char *, kv6data *);
		void getspr(vx5sprite *, const char *);
		kv6data *genmipkv6(kv6data *);
		char *getkfilname(long);
		void animsprite(vx5sprite *, long);
		void drawsprite(vx5sprite *);
		long meltsphere(vx5sprite *, lpoint3d *, long);
		long meltspans(vx5sprite *, vspans *, long, lpoint3d *);

		//Physics helper functions:
		void orthonormalize(point3d *, point3d *, point3d *);
		void dorthonormalize(dpoint3d *, dpoint3d *, dpoint3d *);
		void orthorotate(float, float, float, point3d *, point3d *, point3d *);
		void dorthorotate(double, double, double, dpoint3d *, dpoint3d *, dpoint3d *);
		void axisrotate(point3d *, point3d *, float);
		void slerp(point3d *, point3d *, point3d *, point3d *, point3d *, point3d *, point3d *, point3d *, point3d *, float);
		long cansee(point3d *, point3d *, lpoint3d *);
		void hitscan(dpoint3d *, dpoint3d *, lpoint3d *, long **, long *);
		void sprhitscan(dpoint3d *, dpoint3d *, vx5sprite *, lpoint3d *, kv6voxtype **, float *vsc);
		void hitscan_f(point3d *, point3d *, lpoint3d *, long **, long *);
		void hitscan_side(lpoint3d *, long);
		void sprhitscan_f(point3d *, point3d *, vx5sprite *, lpoint3d *, kv6voxtype **, float *vsc);
		double findmaxcr(double, double, double, double);
		void clipmove(dpoint3d *, dpoint3d *, double);
		void clipmove_float(point3d *, point3d *, float);
		long triscan(point3d *, point3d *, point3d *, point3d *, lpoint3d *);
		void estnorm(long, long, long, point3d *);

		//VXL reading functions (fast!):
		long isvoxelsolid(long, long, long);
		long anyvoxelsolid(long, long, long, long);
		long anyvoxelempty(long, long, long, long);
		long getfloorz(long, long, long);
		long getcube(long, long, long);

		//VXL writing functions (optimized & bug-free):
		void setcube(long, long, long, long);
		void setsphere(lpoint3d *, long, long);
		void setellipsoid(lpoint3d *, lpoint3d *, long, long, long);
		void setcylinder(lpoint3d *, lpoint3d *, long, long, long);
		void setrect(lpoint3d *, lpoint3d *, long);
		void settri(point3d *, point3d *, point3d *, long);
		void setsector(point3d *, long *, long, float, long, long);
		void setspans(vspans *, long, lpoint3d *, long);
		void setheightmap(const unsigned char *, long, long, long, long, long, long, long);
		void setkv6(vx5sprite *, long);

		//VXL writing functions (slow or buggy):
		void sethull3d(point3d *, long, long, long);
		void setlathe(point3d *, long, long, long);
		void setblobs(point3d *, long, long, long);
		void setfloodfill3d(long, long, long, long, long, long, long, long, long);
		void sethollowfill();
		void setkvx(const char *, long, long, long, long, long);
		void setflash(float, float, float, long, long, long);
		void setnormflash(float, float, float, long, long);

		//VXL MISC functions:
		void updatebbox(long, long, long, long, long, long, long);
		void updatevxl();
		void genmipvxl(long, long, long, long);
		void updatelighting(long, long, long, long, long, long);
		void evilquit(const char *);

		//Falling voxels functions:
		void checkfloatinbox(long, long, long, long, long, long);
		void startfalls();
		void dofall(long);
		long meltfall(vx5sprite *, long, long);
		void finishfalls();

		//Procedural texture functions:
		long curcolfunc(lpoint3d *);
		long floorcolfunc(lpoint3d *);
		long jitcolfunc(lpoint3d *);
		long manycolfunc(lpoint3d *);
		long sphcolfunc(lpoint3d *);
		long woodcolfunc(lpoint3d *);
		long pngcolfunc(lpoint3d *);
		long kv6colfunc(lpoint3d *);

		//Editing backup/restore functions
		void voxbackup(long, long, long, long, long);
		void voxdontrestore();
		void voxrestore();
		void voxredraw();

		//High-level (easy) picture loading function:
		void kpzload(const char *, long *, long *, long *, long *);
		//Low-level PNG/JPG functions:
		void kpgetdim(const char *, long, long *, long *);
		long kprender(const char *, long, long, long, long, long, long, long);

		//ZIP functions:
		long kzaddstack(const char *);
		void kzuninit();
		long kzopen(const char *);
		long kzread(void *, long);
		long kzfilelength();
		long kzseek(long, long);
		long kztell();
		long kzgetc();
		long kzeof();
		void kzclose();

		void kzfindfilestart(const char *); //pass wildcard string
		long kzfindfile(char *); //you alloc buf, returns 1:found,0:~found

#ifdef __cplusplus
	}
#endif

#endif

//}