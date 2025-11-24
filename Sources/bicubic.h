// Bicubic from http://groups.google.pl/group/sci.image.processing/browse_thread/thread/bafe941514ec7454/e8d5be7b126036a3%23e8d5be7b126036a3


inline float f(float x)
{
        return (x-floor(x));
}

inline float P(float x)
{
        return (x>0.0f)?x:0.0f;
}

inline float R(float x)
{
	//      return (pow(P(x+2),3) - 4* pow(P(x+1),3) +
	//      6*pow(P(x),3) - 4* pow(P(x-1),3) ) / 6;

	float p0 = P(x+2);
	float p1 = P(x+1);
	float p2 = P(x);
	float p3 = P(x-1);

	return ( (p0*p0*p0) - 4*(p1*p1*p1) + 6*(p2*p2*p2) -	4*(p3*p3*p3) ) / 6.0f; 
}

inline float bicubic(
        float tx, float ty,
        float P00, float P01, float P02, float P03,
        float P10, float P11, float P12, float P13,
        float P20, float P21, float P22, float P23,
        float P30, float P31, float P32, float P33    )
{
    float dx = f(tx);
    float dy = f(ty);

    float Rdx[4], Rdy[4];
    for( int n = 0; n <= 3; n ++ )
    {
        Rdx[n] = R(n-1-dx);
        Rdy[n] = R(n-1-dy);
    }

    float s;

    s =  P00*Rdx[0]*Rdy[0];
    s += P01*Rdx[1]*Rdy[0];
    s += P02*Rdx[2]*Rdy[0];
    s += P03*Rdx[3]*Rdy[0];

    s += P10*Rdx[0]*Rdy[1];
    s += P11*Rdx[1]*Rdy[1];
    s += P12*Rdx[2]*Rdy[1];
    s += P13*Rdx[3]*Rdy[1];
	 
    s += P20*Rdx[0]*Rdy[2];
    s += P21*Rdx[1]*Rdy[2];
    s += P22*Rdx[2]*Rdy[2];
    s += P23*Rdx[3]*Rdy[2];

    s += P30*Rdx[0]*Rdy[3];
    s += P31*Rdx[1]*Rdy[3];
    s += P32*Rdx[2]*Rdy[3];
    s += P33*Rdx[3]*Rdy[3];

    return s;
}

