awk '
  BEGIN {
    width=71
    indent="                                                   ";
  }
  length>width{
    c = 0;
    q = 0;
    x = 0;

    match($0, "^ *");
    spaces = RLENGTH;
    if (spaces <= 0) spaces = 0;

    $0 = substr($0, spaces+1);

    while (length) {
      t = substr($0, 1, width-spaces-(c+x));
      if (length+spaces < width-(c+x)) {
        e = length;
      } else {
        match(t, "^.*[,] *");
        e = RLENGTH;
        if (e <= 0) e = width;
        if (e < 10 || e == width) {
          match(t, "^.*[?:] *");
          e = RLENGTH;
          if (e <= 0) e = width;
          if (e < 10 || e == width) {
            match(t, "^.*[ ] *[^*]");
            e = RLENGTH-1;
            if (e <= 0) e = width;
            if (e < 10 || e == width) {
              match(t, "^.*[-+|&.] *");
              e = RLENGTH-1;
              if (e <= 0) e = width;
            }
          }
        }
      }

      t = substr(t, 1, e)
      $0 = substr($0, e+1);

      if (c == 1) {
        if (length) {
          print substr(indent, 1, spaces) "   " t "\\"
        } else {
          print substr(indent, 1, spaces) "   " t
        }
      } else if (x == 1 && !q) {
        if (length) {
          print substr(indent, 1, spaces) "  " t "\\"
        } else {
          print substr(indent, 1, spaces) "  " t
        }
      } else {
        if (length) {
          print substr(indent, 1, spaces) t "\\"
        } else {
          print substr(indent, 1, spaces) t
        }
      }

      if (match(t, "[/][/]")) c=1;
      if (match(t, "^ *\"")) q=1;
      x = 1;
    }
    next
  }
  {
    print
  }
' $*
