awk '
  BEGIN {i=0; c[0]=1; t[0]=1;}

  function assert(x)
  {
    if (! x) print 1/x;
  }

  # track conditional compilation
  /#if/    {i++; if (($0~"! _WIN32") || ($0~"DEMO") || ($0~"RELEASE") || ($2 == "1")) c[i]=1; else c[i]=0;t[i] = c[i]&&t[i-1]; next}
  /#else/  {c[i]=!c[i];t[i] = c[i]&&t[i-1]; next}
  /#endif/ {assert(i); i--; next}

  # hide conditional compilation
  i&&!t[i]  {next}

  # never show these
  /^ *assert[(]/        {next}

  / *\/\/ *revisit/     {sub(" *\/\/ *revisit.*", "");if (! length) next};

  {print}
' $1
