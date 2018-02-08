set colorcolumn=120
highlight ColorColumn ctermbg=darkblue

augroup project
    autocmd!
    autocmd BufRead,BufNewFile *.h,*.c set filetype=c.doxygen
augroup END

let g:netrw_banner = 0
let g:netrw_liststyle = 3

set tags=./tags;/

" Build inside 'build' directory
set makeprg=make\ -C\ build\ $*

" Show me line numbers for better navigataion and debugging
set number
" Set its width
set numberwidth=2
" Set colors for the bar
set t_Co=256
highlight LineNr term=bold cterm=NONE ctermfg=lightyellow ctermbg=darkgrey gui=NONE guifg=DarkGrey guibg=NONE

" Make split always equal
set equalalways
