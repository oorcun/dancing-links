/**
 * Written by Orçun Altınsoy on April 2011
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROW 6
#define COLUMN 7

#define CSIZE 4 // sütun isimleri boyutu
#define SSIZE 16 // çözüm boyutu
#define INFINITY 0x7FFFFFF

int example[ROW][COLUMN] = {{0 , 0 , 1 , 0 , 1 , 1 , 0} ,
                            {1 , 0 , 0 , 1 , 0 , 0 , 1} , 
                            {0 , 1 , 1 , 0 , 0 , 1 , 0} ,
                            {1 , 0 , 0 , 1 , 0 , 0 , 0} , 
                            {0 , 1 , 0 , 0 , 0 , 0 , 1} ,
                            {0 , 0 , 0 , 1 , 1 , 0 , 1}};

struct data
{
       int row; // her data nesnesi bulunduğu satır ve sütun numarasını tutuyor (zorunlu değil -
       int column; // - ama algoritmayı takip etmek mümkün ve kodlar daha anlaşılır oluyor)  
       data *L; // sol
       data *R; // sağ
       data *U; // yukarı
       data *D; // aşağı
       data *C; // sütun başlığı
       int S; // sütundaki 1'lerin sayısı (yalnızca sütun başlığında bulunuyor)
       char N[CSIZE]; // sütun ismi
};

data *matrix[ROW + 1][COLUMN + 1]; 
// bellek büyük problem olduğu için matrix'de 0 olan yerlerde bellek tahsis edilmemeli
data *h; // kök (yalnızca L ve R elemanları kullanılıyor)

data *O[SSIZE]; // çözümler
 
void initialize() 
{
     int row , column;
     int i;
     data *c , *r;
// matrix için yer ayırma işlemleri (row ve column elemanları da atanıyor)
     for(row = 0 ; row <= ROW ; row++) for(column = 0 ; column <= COLUMN ; column++)
      if(!row)
      {
              matrix[row][column] = (data*)malloc(sizeof(data));
              matrix[row][column] -> row = row;
              matrix[row][column] -> column = column;
      }
      else if(row && column && example[row - 1][column - 1])
      {
           matrix[row][column] = (data*)malloc(sizeof(data));
           matrix[row][column] -> row = row;
           matrix[row][column] -> column = column;
      }    
// matrix bağları oluşturuluyor (0. satır = sütun başlıkları)    
     for(row = 0 ; row <= ROW ; row++) for(column = 0 ; column <= COLUMN ; column++) 
      if(matrix[row][column])
      { 
                             i = column;
                             do i ? --i : i = COLUMN; while(!matrix[row][i]);
                             matrix[row][column] -> L = matrix[row][i];
             
                             i = column; 
                             do ++i %= COLUMN + 1; while(!matrix[row][i]);
                             matrix[row][column] -> R = matrix[row][i];
             
                             i = row; 
                             do i ? --i : i = ROW; while(!matrix[i][column]); 
                             matrix[row][column] -> U = matrix[i][column];
             
                             i = row;
                             do ++i %= ROW + 1; while(!matrix[i][column]);
                             matrix[row][column] -> D = matrix[i][column];
             
                             if(row) matrix[row][column] -> C = matrix[0][column];
     }
// sütun başlıklarına isimler atanıyor     
     strcpy(matrix[0][1] -> N , "A");
     strcpy(matrix[0][2] -> N , "B");
     strcpy(matrix[0][3] -> N , "C");
     strcpy(matrix[0][4] -> N , "D");
     strcpy(matrix[0][5] -> N , "E");
     strcpy(matrix[0][6] -> N , "F");
     strcpy(matrix[0][7] -> N , "G");
// sütun büyüklükleri oluşturuluyor
     h = matrix[0][0];    
     for(c = h -> R ; c != h ; c = c -> R)
     {
           c -> S = 0;
           for(r = c -> D ; r != c ; r = r -> D) c -> S++;
     }
}

void showlinks() // kontrol amaçlı (zorunlu değil) bu fonksiyon istenilen yerde çağırılıp -
{ // - linklerin durumu kontrol edilebilir
     int row , column;
     
     printf("left links\n");
     for(row = 0 ; row <= ROW ; row++)
     {
             for(column = 0 ; column <= COLUMN ; column++) if(matrix[row][column])
              printf("(%d , %d) <- (%d , %d)  " , 
              matrix[row][column] -> row , matrix[row][column] -> column , 
              matrix[row][column] -> L -> row , matrix[row][column] -> L -> column);
             printf("\n");
     }
     
     printf("right links\n");
     for(row = 0 ; row <= ROW ; row++)
     {
             for(column = 0 ; column <= COLUMN ; column++) if(matrix[row][column])
              printf("(%d , %d) -> (%d , %d)  " , 
              matrix[row][column] -> row , matrix[row][column] -> column , 
              matrix[row][column] -> R -> row , matrix[row][column] -> R -> column);
             printf("\n");
     }
     
     printf("up links\n");
     for(column = 0 ; column <= COLUMN ; column++)
     {
                for(row = 0 ; row <= ROW ; row++) if(matrix[row][column])
                 printf("(%d , %d) /\\ (%d , %d)  " , 
                 matrix[row][column] -> row , matrix[row][column] -> column , 
                 matrix[row][column] -> U -> row , matrix[row][column] -> U -> column);
                printf("\n");
     }
     
     printf("down links\n");
     for(column = 0 ; column <= COLUMN ; column++)
     {
                for(row = 0 ; row <= ROW ; row++) if(matrix[row][column])
                 printf("(%d , %d) \\/ (%d , %d)  " , 
                 matrix[row][column] -> row , matrix[row][column] -> column , 
                 matrix[row][column] -> D -> row , matrix[row][column] -> D -> column);
                printf("\n");
     }
     
     printf("headers\n");
     for(row = 1 ; row <= ROW ; row++)
     {
             for(column = 0 ; column <= COLUMN ; column++) if(matrix[row][column])
              printf("(%d , %d) 's (%d , %d)  " , 
              matrix[row][column] -> row , matrix[row][column] -> column , 
              matrix[row][column] -> C -> row , matrix[row][column] -> C -> column);
             printf("\n");
     }
     
     printf("names\n");
     for(column = 1 ; column <= COLUMN ; column++) if(matrix[0][column])
      printf("(%d , %d) 's %s    " , 
             matrix[0][column] -> row , matrix[0][column] -> column , matrix[0][column] -> N);
     printf("\n");
     
     printf("sizes\n");
     for(column = 1 ; column <= COLUMN ; column++) if(matrix[0][column])
      printf("(%d , %d) 's %d    " , 
             matrix[0][column] -> row , matrix[0][column] -> column , matrix[0][column] -> S);
     printf("\n"); 
}

void print(int k) // çözümü yaz
{
     printf("SOLUTION FOUND\n");
     
     int i;
     data *r;
     
     for(i = 0 ; i < k ; i++)
     {
           printf("%s " , O[i] -> C -> N);
           for(r = O[i] -> R ; r != O[i] ; r = r -> R) printf("%s " , r -> C -> N);
           printf("\n");
     }
}

data* choose() // sütun seç
{
      int s = INFINITY;
      data *c;
      data *j;
// en küçük ilk sütun seçiliyor      
      for(j = h -> R ; j != h ; j = j -> R) if(j -> S < s)
      {
            c = j;
            s = j -> S;
      }
      
      return c;
}

void cover(data *c)
{
     printf("covering column %s\n" , c -> N);
     
     data *i;
     data *j;
     
     printf("removing (%d , %d) horizontally\n" , c -> row , c -> column);
     c -> R -> L = c -> L;
     c -> L -> R = c -> R;
     
     for(i = c -> D ; i != c ; i = i -> D) for(j = i -> R ; j != i ; j = j -> R)
     {
           printf("removing (%d , %d) vertically\n" , j -> row , j -> column);
           j -> D -> U = j -> U;
           j -> U -> D = j -> D;
           j -> C -> S--;
     }
     
     printf("finished covering column %s\n" , c -> N);
     getchar();
}

void uncover(data *c)
{
     printf("uncovering column %s\n" , c -> N);
     
     data *i;
     data *j;
     
     for(i = c -> U ; i != c ; i = i -> U) for(j = i -> L ; j != i ; j = j -> L)
     {
           printf("putting back (%d , %d) vertically\n" , j -> row , j -> column);
           j -> C -> S++;
           j -> D -> U = j;
           j -> U -> D = j;
     }
     
     printf("putting back (%d , %d) horizontally\n" , c -> row , c -> column);
     c -> R -> L = c;
     c -> L -> R = c;
     
     printf("finished uncovering column %s\n" , c -> N);
     getchar();
}

void search(int k)
{
     printf("entering search(%d)\n" , k);
     
     if(h -> R == h)
     {
          print(k);
          return;
     }
     
     data *r;
     data *j;
     data *c = choose();
     
     printf("choosing column %s\n" , c -> N);
     
     cover(c);
     
     for(r = c -> D ; r != c ; r = r -> D)
     {
           O[k] = r; // çözüm adayı
           printf("choosing row (%d , %d) for temporary solution\n" , r -> row , r -> column);
           for(j = r -> R ; j != r ; j = j -> R) cover(j -> C);
           search(k + 1);
           r = O[k];
           printf("reattaining row (%d , %d)\n" , r -> row , r -> column);
           c = r -> C;
           for(j = r -> L ; j != r ; j = j -> L) uncover(j -> C);
     }
     uncover(c);
     
     printf("exiting search(%d)\n" , k);
}

int main()
{
    initialize();
    showlinks();
    search(0);
    
    printf("ALL SOLUTIONS FOUND\n");
    getchar();
}
