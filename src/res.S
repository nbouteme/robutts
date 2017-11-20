	.section .filetable, "aw"
fnptr:	.quad filename
filesize:	.quad file_end-file

	.section .filedata, "aw"
filename:	.string NAME
file:	.incbin FILE
file_end:
	