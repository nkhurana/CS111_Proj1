sort < a | cat b - | trA-Z a-z > c
sort -k2 d - < a | uniq -c > e
diff a c > f
