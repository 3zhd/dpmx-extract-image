
//modified and referred exdpmx.cpp


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct DPMXHDR {
  unsigned char signature[4];
  unsigned long data_offset;
  unsigned long entry_count;
  unsigned long unknown1;
};

struct DPMXENTRY {
  char          filename[16];
  unsigned long unknown1; // 0xFFFFFFFF
  unsigned long seed;
  unsigned long offset;
  unsigned long length;
};


unsigned char jpegheaderdata[3]
={0xFF, 0xD8, 0xFF};
//FF D8 FF is common Jpeg header 
unsigned char Exifheaderdata[4]
={0x45,0x78,0x69,0x66};
//Exif, header address 0x06-0x09
unsigned char JFIFheaderdata[4]
={0x4A ,0x46,0x49,0x46};
//JFIF, header address 0x06-0x09
unsigned char BMPheaderdata[14]
={0x42, 0x4D, 0x36, 0xF9, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00};
//BM, header address 0x00-0x0D, only first 2 bytes are signiture



unsigned char BMPSIZEheaderdata[4]
={0x00, 0x00, 0x00, 0x00};
//BMP size in byte, header address 0x02-0x05

/*
unsigned char DPMBPMEOI[2]
={0x00, 0x00};
//BMP in dpm EOI
*/

unsigned char rawheaderdata[14]; //read from file
unsigned char decryptedheaderdata[14]; //tmp array for calculation

unsigned char eofbuff[2];
unsigned char fileeof[2];
unsigned char EOIOREOF[2]
={0xFF, 0xD9};//jpeg End Of Image

unsigned char mutator1=0x00;
unsigned char mutator2=0x00;
//0x00 - 0xFF

/*
(i=0)decryptedheaderdata[i]=(mutator1^rawheaderdata[i])-mutator2;
(else)decryptedheaderdata[i]=decryptedheaderdata[i-1]+((mutator1^rawheaderdata[i])-mutator2);
*/

//


void unobfuscate(unsigned char* buff,
                 unsigned long  len,
                 unsigned char* mutator1,
                 unsigned char* mutator2)
{

  unsigned char key= 0;
  
  
  for(int t=0;t<len;t++)
   { 
    if(t!=0)
    buff[t]=buff[t-1]+((*mutator1) ^buff[t])- (*mutator2);
    else
    buff[t]=((*mutator1) ^buff[t])- (*mutator2);
  }
  
  
}



void bforce( unsigned char* mutator1, unsigned char* mutator2,int* countcheck, int* countcheck2
,unsigned char* buff, unsigned long len,struct DPMXENTRY* entries[],int* ecount, int* countnot,FILE* fd,unsigned long offstart)
{ FILE* fdw;//fd write
  FILE* fdcheck;//check End Of Image
 (*mutator1)=0x00;
 (*mutator2)=0x00;
 (*countcheck)=0;
 (*countcheck2)=0;
int l=0;
int ll=0;


 for(l=0;l<256;l++)
 { (*mutator2)=l;
            for(ll=0;ll<256;ll++)
            {   
                (*mutator1)=ll; 
              //read to buff EVERY EACH TIME
              fseek(fd,offstart, SEEK_SET);
              fread(buff,len,1,fd);
              memcpy(rawheaderdata,buff,14);    
        
                for(int j=0;j<14;j++)
                {     
                      if(j==0)
                      {
                      decryptedheaderdata[j]=((*mutator1)^rawheaderdata[j])-(*mutator2);
                      }
                      else
                      {
                      decryptedheaderdata[j]=decryptedheaderdata[j-1]+((*mutator1)^rawheaderdata[j])-(*mutator2);
                      }

                              
                } 
                
                      //convert unsigned long len to unsinged char array
                      
                      BMPSIZEheaderdata[0]=(len & 0x000000FF);
                      BMPSIZEheaderdata[1]=(len & 0x0000FF00)>>8;
                      BMPSIZEheaderdata[2]=(len & 0x00FF0000)>>16;
                      BMPSIZEheaderdata[3]=(len & 0xFF000000)>>24;
                       
                      (*countcheck)=0;
                      for(int k=0;k<2;k++)
                      { if(decryptedheaderdata[k]==BMPheaderdata[k])
                       {(*countcheck)=(*countcheck)+1;}
                      }
                      for(int k=6;k<14;k++)
                      { if(decryptedheaderdata[k]==BMPheaderdata[k])
                       {(*countcheck)=(*countcheck)+1;}
                      }
                      
                      for(int k=0;k<4;k++)
                      { if(decryptedheaderdata[k+2]==BMPSIZEheaderdata[k])
                       {(*countcheck)=(*countcheck)+1;}
                      }
                       
             

              if((*countcheck)==14)//size, offset header differs
              { 
                  unobfuscate(buff,len,mutator1,mutator2);
                  fdw=fopen(entries[(*ecount)]->filename,"w+b");
                  fwrite(buff,len,1,fdw);
                  fclose(fdw);
                  fflush(fdw);
                  
                  
                  /*
                  fdcheck=fopen(entries[(*ecount)]->filename,"rb");
                  fseek(fdcheck,-2,SEEK_END);
                  fread(eofbuff,2,1,fdcheck); 
                  fclose(fdcheck);
                  fflush(fdcheck);
                  memcpy(fileeof,eofbuff,2);*/
                  break;
                  //(*countcheck2)=0;                
                  
                  /*
                  for(int k=0;k<2;k++)
                   {
                     if(fileeof[k]==DPMBPMEOI[k])
                     {(*countcheck2)=(*countcheck2)+1;}
                      
                   }
                */
               /*
                  if((*countcheck2)==2)
                  {
                    break;}  
                   else
                  {(*countcheck)=0;
                  (*countcheck2)=0;
                   }*/
              }    
              
                
                
             
             
                          
               


               
                
            }



            
    

         if((*countcheck)==14)
         {
                
                 printf("\r\nchecked all count: %d ",(*countcheck));
                 (*countcheck)=0;
                  
                  
                 return;
         }
         else if((l==255)&&(ll==256)&&((*countcheck)!=14))
         {
                
              
               (*countnot)=(*countnot)+1;
               
               printf("\r\n NOT FOUND\r\n");

              (*countcheck)=0;
              
                return;
              
         }
         else
         { 
              (*countcheck)=0;
              //(*countcheck2)=0;
              //reset counter
              
              
              
         }
            
            
 

 }



}














int main(int argc,char** argv)
{



int countnot=0;


    if (argc != 2) {
    
    printf("err. input a file");

    return -1;
    }
   FILE* fd;
   

  char* filename[1]; 
   
  struct DPMXHDR hdr;

   //open
   
  
  filename[0]=malloc(sizeof(char)*strlen(argv[1])+1);
  memcpy(filename[0],argv[1],sizeof(char)*strlen(argv[1]));
  //need free
 

  fd=fopen(filename[0],"rb");
  //read(fd, &of buf, byte);
   
   
   
   
   if(fd!=NULL)
   /*check header */
   { //dpmx header check here
     
     fread(&hdr,sizeof(hdr),1,fd);
     
     printf("%s\r\n",hdr.signature);
     //if dpmx header
     if(!memcmp(hdr.signature, "DPMX", 4))
     {
       printf("it's dpm file");

      if(!(hdr.data_offset == hdr.entry_count * sizeof(struct DPMXENTRY) + sizeof(hdr)))
      {
        printf("file entry info is wrong");
        fclose(fd);
        free(filename[0]);
        return -1;
      }
      


     }
     else
     {
       printf("it's not dpm file");
       fclose(fd);
       free(filename[0]);
       return -1;   
     }
      //file check done


   
    //read entry
    struct DPMXENTRY* entries[hdr.entry_count];
    
    for(int i=0;i<hdr.entry_count;i++)
    { 
      entries[i]=malloc(sizeof(struct DPMXENTRY));
      fread(entries[i],sizeof(struct DPMXENTRY),1,fd);
    }//read entries
    
    
    //go to each file using offsets
    printf("\r\n%d\r\n",hdr.entry_count);
    for (unsigned int i = 0; i <hdr.entry_count; i++) // 2;i++)//
    {
    unsigned long  len  = entries[i]->length;
    unsigned char* buff;
    buff=malloc(sizeof(char)*len);//need to free
    fseek(fd, ((hdr.data_offset) + (entries[i]->offset)), SEEK_SET);
       

   
    int countcheck=0;
    int countcheck2=0;
    /*brute force each file*/
      
    mutator1=0x00;
    mutator2=0x00;
    printf("\r\nfilename: %s", entries[i]->filename);
    
    bforce(&mutator1,&mutator2,&countcheck,&countcheck2,buff,len,entries,&i,&countnot,fd,((hdr.data_offset) + (entries[i]->offset)));




  
    
 
  free(buff); 
    
  }
    
     
      
     






      for(int i=0;i<hdr.entry_count;i++)
      {
      free(entries[i]);
      }


 }
 else
 {
  printf("file open failed");
  free(filename[0]);
  fclose(fd);
  return -1;
} 

   


  
  
   free(filename[0]);
   fclose(fd);
   
   printf("\r\n%d files not found mutator",countnot);
   getchar();

return 0;

}
