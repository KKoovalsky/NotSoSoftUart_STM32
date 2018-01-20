set colorcolumn=120
highlight ColorColumn ctermbg=darkblue

augroup project
    autocmd!
    autocmd BufRead,BufNewFile *.h,*.c set filetype=c.doxygen
augroup END

let g:netrw_banner = 0
let g:netrw_liststyle = 3

