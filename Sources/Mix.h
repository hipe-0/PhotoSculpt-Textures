
inline float mix ( float a, float b, float amount)
{
  return a * amount + b * (1.0f - amount);
}
inline int mix ( int a, int b, float amount)
{
  return round ( float(a) * amount + float(b) * (1.0f - amount) );
}
