augroup LOCAL_SETTINGS_
  autocmd BufEnter *.c,*.h nnoremap <silent><space>bf :call CocAction('format') <CR>
augroup END
