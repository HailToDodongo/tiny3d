// Minimal ELF32 big-endian reader for the RSP ucode ELFs (rsp.ld layout):
// section bytes by name (.text -> IMEM, .data -> DMEM) and the symbol table.
import {readFileSync} from 'node:fs';

/**
 * @param {string} path
 * @returns {{sections: Object, symbols: Object<string, number>, bytes: (name: string) => Uint8Array}}
 *   symbols hold the full link address (e.g. 0xA4001280); mask with 0x1FFF for the RSP address
 */
export function parseElf(path) {
  const buf = readFileSync(path);
  const dv = new DataView(buf.buffer, buf.byteOffset, buf.byteLength);
  const u16 = (o) => dv.getUint16(o, false);
  const u32 = (o) => dv.getUint32(o, false);
  if (u32(0) !== 0x7F454C46) throw new Error('not an ELF file: ' + path);

  const shoff = u32(0x20), shentsize = u16(0x2E), shnum = u16(0x30), shstrndx = u16(0x32);
  const sh = (i) => {
    const o = shoff + i * shentsize;
    return {name: u32(o), type: u32(o + 4), addr: u32(o + 12), offset: u32(o + 16),
            size: u32(o + 20), link: u32(o + 24), entsize: u32(o + 36)};
  };
  const cstr = (base, off) => {
    let e = base + off; while (buf[e] !== 0) e++;
    return buf.toString('latin1', base + off, e);
  };
  const strtab = sh(shstrndx).offset;
  const sections = {};
  const list = [];
  for (let i = 0; i < shnum; i++) { const s = sh(i); s.strName = cstr(strtab, s.name); sections[s.strName] = s; list.push(s); }

  const symbols = {};
  const symtab = sections['.symtab'];
  if (symtab) {
    const names = list[symtab.link].offset;
    for (let o = symtab.offset; o < symtab.offset + symtab.size; o += 16) {
      const name = cstr(names, u32(o));
      if (name) symbols[name] = u32(o + 4);
    }
  }
  const bytes = (name) => {
    const s = sections[name];
    if (!s || s.type === 8 /*NOBITS*/) return new Uint8Array(0);
    return new Uint8Array(buf.buffer, buf.byteOffset + s.offset, s.size);
  };
  return {sections, symbols, bytes};
}

