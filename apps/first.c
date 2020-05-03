int main() {
  int n = f(2);
  int x = 0;
  for (int i = 0; i <= n; ++i) {
    x += i;
  }
  return x;
}

int f(int k) {
  int x = 1;
  for (int i = 0; i < k; ++i) {
    x *= 2;
  }
  return x;
}
