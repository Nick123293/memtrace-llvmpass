
int main()
{
  int *a = new int[10];
  int *b = new int[10];
  for (int i = 0; i<10; i++)
  {
    a[i] = 0;
    b[i] = 1;
  }
  for(int j = 0; j<10; j++)
  {
    a[j] = b[j] + 1;
  }
  return 0;
}
