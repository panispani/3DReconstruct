import os

dirs = ['background','ball','bottle','bull','deer','mug','pringles','raspberry','scissors','winebottle']
subdirs = ['scan1', 'scan2', 'scan3', 'scanshaky', 'scanclose']
for dir in dirs:
	for subdir in subdirs:
		os.system('mkdir {}\\{}\\color'.format(dir,subdir))
		os.system('mkdir {}\\{}\\depth'.format(dir,subdir))
		os.system('"C:\\Users\\Yifei\\Downloads\\ffmpeg\\ffmpeg-20200401-afa5e38-win64-static\\bin\\ffmpeg.exe" -i {}\\{}\\out.mkv -map 0:0 -r 5 {}\\{}\\color\\color%04d.png'.format(dir,subdir, dir,subdir))
		os.system('"C:\\Users\\Yifei\\Downloads\\ffmpeg\\ffmpeg-20200401-afa5e38-win64-static\\bin\\ffmpeg.exe" -i {}\\{}\\out.mkv -map 0:1 -r 5 {}\\{}\\depth\\depth%04d.png'.format(dir, subdir,dir, subdir))