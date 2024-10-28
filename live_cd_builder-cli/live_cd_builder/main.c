/*

  Este programa empaqueta un sistema
  de archivos GNU/Linux en un .iso hibrido booteable
  para sistemas MBR.

  Creado por Haylem Candelario Bauza
  Software Libre GPLv2. 2024. Cuba.

*/

void copyfs(const char *org);
void run_scripts(void);
void compress_fs(void);
void copy_kernel(void);
void copy_bootloader(void);
void generate_iso(void);
void restore_bashrc(void);


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char pathfs[5000] = {'\0'};

  system("clear");
  printf("Ruta de carpeta del sistema de archivos montado\n-> ");
  scanf("%s", pathfs);

  copyfs(pathfs);
  run_scripts();
  compress_fs();
  copy_kernel();
  copy_bootloader();
  generate_iso();

  return 0;
}


void generate_iso(void)
{

  printf("\nGenerando .iso...\n");
  system("rm  /tmp/gnulinux-live.iso 2>/dev/null");

  system("./data/bin/genisoimage -v -r -V \"gnu-linux\" -allow-limited-size -cache-inodes \
                      -J -l -b isolinux/isolinux.bin -c isolinux/boot.cat \
                      -no-emul-boot -boot-load-size 4 -boot-info-table \
                       -o /tmp/gnulinux-live.iso /tmp/live-iso >/dev/null");


  system("./data/bin/isohybrid /tmp/gnulinux-live.iso");

}

void copy_bootloader(void)
{
  printf("\n\nCopiando cargador de arranque...\n");
  system("cp -rv ./data/configs/isolinux /tmp/live-iso/");
}

void compress_fs(void)
{

  printf("\n\n\nComprimiendo sistema de archivos...\n");
  system("rm -r /tmp/live-iso 2>/dev/null");
  system("mkdir /tmp/live-iso");
  system("mkdir /tmp/live-iso/live");
  system("./data/bin/mksquashfs /tmp/temp-filesystem /tmp/live-iso/live/filesystem.squashfs -b 1M -comp xz -Xbcj x86 >/dev/null");
}

void copy_kernel(void)
{
  printf("\n\nCopiando kernel...\n");
  system("cp -v /tmp/temp-filesystem/boot/vmlinu* /tmp/live-iso/live/kernel");
  system("cp -v /tmp/temp-filesystem/boot/initr* /tmp/live-iso/live/initrd");
}

void run_scripts(void)
{
  char cmd[5000] = {'\0'};

  printf("Copiando configuracion bashrc...\n");
  sprintf(cmd, "cp -fv ./data/configs/bashrc '%s/root/.bashrc' >/dev/null", "/tmp/temp-filesystem");
  system(cmd);

  printf("Copiando .debs de livecd...\n");
  sprintf(cmd, "cp -fv ./data/debs/* %s/tmp/ >/dev/null", "/tmp/temp-filesystem");
  system(cmd);


  printf("Ejecutando script instalacion de .debs...\n");
  sprintf(cmd, "chroot '%s'", "/tmp/temp-filesystem");
  system(cmd);

  printf("Borrando debs residuales...\n");
  sprintf(cmd, "rm -v %s/tmp/*.deb 2>/dev/null", "/tmp/temp-filesystem");
  system(cmd);

  system("cp -v ./data/configs/fstab /tmp/temp-filesystem/etc/fstab");

  restore_bashrc();

}

void restore_bashrc(void)
{
  printf("Restaurando bashrc ...");
  system("cp -fv ./data/configs/bashrc-orig /tmp/temp-filesystem/root/.bashrc");
}


void copyfs(const char *org)
{
  char cmd[5000] = {'\0'};


  printf("\nEliminando carpeta pre-existente, espere...\n");
  system("rm -r /tmp/temp-filesystem 2>/dev/null");

  printf("\nCopiando sistema de archivos, espere...\n");
  system("mkdir /tmp/temp-filesystem");
  sprintf(cmd, "./data/bin/rsync -a %s/* /tmp/temp-filesystem", org);
  system(cmd);

  printf("Borrando carpetas residuales...\n");
  system("rm -rv /tmp/temp-filesystem/dev >/dev/null");
  system("rm -rv /tmp/temp-filesystem/proc >/dev/null");
  system("rm -rv /tmp/temp-filesystem/sys >/dev/null");
  system("rm -rv /tmp/temp-filesystem/run >/dev/null");
  system("rm -rv /tmp/temp-filesystem/media >/dev/null");
  system("rm -rv /tmp/temp-filesystem/tmp >/dev/null");


  printf("Recreando carpetas limpias...\n");
  system("mkdir /tmp/temp-filesystem/dev");
  system("mkdir /tmp/temp-filesystem/proc");
  system("mkdir /tmp/temp-filesystem/sys");
  system("mkdir /tmp/temp-filesystem/run");
  system("mkdir /tmp/temp-filesystem/media");
  system("mkdir /tmp/temp-filesystem/tmp");

}
