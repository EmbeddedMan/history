for i in [!w]*.c; do q=${i%.c}; for j in $q.h $q.c; do; if [ -f $j ]; then real.sh $j | fold.sh; fi; done; done >real.txt

cat ../lcf/MCF52221_INTERNAL_FLASH.lcf >>real.txt

awk 'length>n{n=length}length==73{print}END{print n}' real.txt
