let SessionLoad = 1
if &cp | set nocp | endif
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
doautoall SessionLoadPre
silent only
silent tabonly
cd ~/Documentos/UTN/TecnicasDigitales2/ep1/sources/up.svproj
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
set shortmess+=aoO
badd +54 src/up_harvard.sv
badd +1 src/alu.sv
badd +1 src/ram3port.sv
badd +1 src/shifter.sv
badd +1 reg_queue.sv
badd +11 src/reg_queue.sv
badd +1 src/datapath.sv
badd +0 tb/up_harvard_tb.sv
badd +1 tb/up_harvard_mem.txt
badd +1 WARNING:\ tb/up_harvard_tb.sv
argglobal
%argdel
$argadd src/up_harvard.sv
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit src/shifter.sv
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
2wincmd h
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 30 + 32) / 64)
exe 'vert 1resize ' . ((&columns * 72 + 160) / 320)
exe '2resize ' . ((&lines * 30 + 32) / 64)
exe 'vert 2resize ' . ((&columns * 72 + 160) / 320)
exe 'vert 3resize ' . ((&columns * 71 + 160) / 320)
exe 'vert 4resize ' . ((&columns * 175 + 160) / 320)
argglobal
balt src/ram3port.sv
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 1 - ((0 * winheight(0) + 15) / 30)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("src/ram3port.sv", ":p")) | buffer src/ram3port.sv | else | edit src/ram3port.sv | endif
balt src/shifter.sv
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
1
sil! normal! zo
let s:l = 8 - ((7 * winheight(0) + 15) / 30)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 8
normal! 025|
wincmd w
argglobal
if bufexists(fnamemodify("src/alu.sv", ":p")) | buffer src/alu.sv | else | edit src/alu.sv | endif
balt src/ram3port.sv
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 26 - ((25 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 26
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("src/up_harvard.sv", ":p")) | buffer src/up_harvard.sv | else | edit src/up_harvard.sv | endif
balt src/reg_queue.sv
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
1
sil! normal! zo
25
sil! normal! zo
let s:l = 17 - ((16 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 17
normal! 040|
wincmd w
exe '1resize ' . ((&lines * 30 + 32) / 64)
exe 'vert 1resize ' . ((&columns * 72 + 160) / 320)
exe '2resize ' . ((&lines * 30 + 32) / 64)
exe 'vert 2resize ' . ((&columns * 72 + 160) / 320)
exe 'vert 3resize ' . ((&columns * 71 + 160) / 320)
exe 'vert 4resize ' . ((&columns * 175 + 160) / 320)
tabnext
edit tb/up_harvard_mem.txt
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 128 + 160) / 320)
exe 'vert 2resize ' . ((&columns * 191 + 160) / 320)
argglobal
balt WARNING:\ tb/up_harvard_tb.sv
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 6 - ((5 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 6
normal! 06|
wincmd w
argglobal
if bufexists(fnamemodify("tb/up_harvard_tb.sv", ":p")) | buffer tb/up_harvard_tb.sv | else | edit tb/up_harvard_tb.sv | endif
balt tb/up_harvard_mem.txt
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
18
sil! normal! zo
38
sil! normal! zo
let s:l = 42 - ((41 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 42
normal! 016|
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 128 + 160) / 320)
exe 'vert 2resize ' . ((&columns * 191 + 160) / 320)
tabnext
edit src/datapath.sv
argglobal
balt src/reg_queue.sv
setlocal fdm=expr
setlocal fde=b:anyfold_ind_buffer[v:lnum-1]
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
1
sil! normal! zo
let s:l = 1 - ((0 * winheight(0) + 31) / 62)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
tabnext 2
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
