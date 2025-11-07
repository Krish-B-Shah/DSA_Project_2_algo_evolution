// plain js no dependencies to worry about.
(function(){
  const fileInput = document.getElementById('fileInput');
  const playPauseBtn = document.getElementById('playPauseBtn');
  const resetBtn = document.getElementById('resetBtn');
  const prevBtn = document.getElementById('prevBtn');
  const nextBtn = document.getElementById('nextBtn');
  const speedSlider = document.getElementById('speedSlider');
  const optFilter = document.getElementById('optFilter');
  const algoFilter = document.getElementById('algoFilter');
  const revealMode = document.getElementById('revealMode');
  const colorMode = document.getElementById('colorMode');
  const csvUrlInput = document.getElementById('csvUrl');
  const loadUrlBtn = document.getElementById('loadUrlBtn');
  const loadKaggleBtn = document.getElementById('loadKaggleBtn');

  const canvas = document.getElementById('vizCanvas');
  const ctx = canvas.getContext('2d', { alpha: false });
  const stepStat = document.getElementById('stepStat');
  const countStat = document.getElementById('countStat');
  const bestFitnessStat = document.getElementById('bestFitnessStat');
  const bestDNA = document.getElementById('bestDNA');
  const spaceStat = document.getElementById('spaceStat');
  const lbQS = document.getElementById('lbQS');
  const lbMS = document.getElementById('lbMS');
  const lbGA = document.getElementById('lbGA');
  const lbSA = document.getElementById('lbSA');

  const modal = document.getElementById('modal');
  const modalClose = document.getElementById('modalClose');
  const modalBody = document.getElementById('modalBody');

  let isPlaying = false;
  let speed = 1.0;
  let lastTs = 0;
  let currentStep = 0;
  let lastRenderStep = -1;
  let cachedFiltered = null;

  // points array
  let points = [];
  let stepsMin = 0, stepsMax = 0;
  let minComp = Infinity, maxComp = -Infinity;
  let minSwp = Infinity, maxSwp = -Infinity;
  let minFit = Infinity, maxFit = -Infinity;
  let isHeavyDataset = false;
  const fitnessQuantilesByStep = new Map();

  // resize stuff
  function resizeCanvas(){
    const dpr = Math.max(1, Math.min(2, window.devicePixelRatio || 1));
    const rect = canvas.getBoundingClientRect();
    const w = Math.max(400, rect.width);
    const h = Math.max(300, rect.height);
    const newWidth = Math.floor(w * dpr);
    const newHeight = Math.floor(h * dpr);
    
    if(canvas.width !== newWidth || canvas.height !== newHeight){
      canvas.width = newWidth;
      canvas.height = newHeight;
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      render.bgGradient = null;
      cachedFiltered = null;
    }
  }
  
  let resizeTimeout = null;
  window.addEventListener('resize', () => {
    if(resizeTimeout) clearTimeout(resizeTimeout);
    resizeTimeout = setTimeout(resizeCanvas, 100);
  });
  
  resizeCanvas();

  // file input
  fileInput.addEventListener('change', (e) => {
    const file = e.target.files && e.target.files[0];
    if(!file) return;
    const reader = new FileReader();
    reader.onload = () => parseCSV(reader.result);
    reader.readAsText(file);
  });

  loadUrlBtn.addEventListener('click', () => {
    const url = (csvUrlInput.value || '').trim();
    if(!url) return;
    loadCSVFromUrl(url);
  });

  const DEMO_CANDIDATES = [
    '/data/logs/demo_qs_ms.csv',
    '/data/logs/full_test.csv',
    '/data/logs/viral_data.csv',
    '/data/logs/run.csv',
    '/data/logs/massive_evolution.csv',
    '../data/logs/demo_qs_ms.csv',
    '../data/logs/massive_evolution.csv',
    '../data/logs/full_test.csv',
    '../data/logs/viral_data.csv',
    '../data/logs/run.csv'
  ];
  loadKaggleBtn.addEventListener('click', async () => {
    const ts = '?t=' + Date.now();
    for(const url of DEMO_CANDIDATES){
      try{
        const res = await fetch(url + ts, { cache: 'no-store' });
        if(!res.ok) throw new Error('HTTP ' + res.status);
        const text = await res.text();
        csvUrlInput.value = url;
        localStorage.setItem('viz_csv_url', url);
        parseCSV(text);
        return;
      }catch(e){ 
        // try next
      }
    }
    alert('Demo CSV not found under /data/logs. Use Load with a URL or run scripts/download_kaggle.py');
  });
  csvUrlInput.value = localStorage.getItem('viz_csv_url') || '';
  csvUrlInput.addEventListener('change', () => { localStorage.setItem('viz_csv_url', csvUrlInput.value || ''); });

  function parseCSV(text){
    const lines = text.split(/\r?\n/).filter(Boolean);
    if(lines.length < 2) return;
    const header = lines[0].split(',').map(s => s.trim());
    const colIndex = Object.fromEntries(header.map((h, i) => [h, i]));

    const isKaggle = 'run_id' in colIndex && 'step' in colIndex && 'algo' in colIndex && 'opt' in colIndex && 'fitness_ms' in colIndex;

    points = [];
    minComp = Infinity; maxComp = -Infinity;
    minSwp = Infinity; maxSwp = -Infinity;
    minFit = Infinity; maxFit = -Infinity;
    stepsMin = Infinity; stepsMax = -Infinity;

    for(let i = 1; i < lines.length; i++){
      const cells = lines[i].split(',');
      if(cells.length < 4) continue;
      if(isKaggle){
        const stepStr = cells[colIndex.step] || '0';
        if(stepStr === 'step' || isNaN(stepStr)) continue;
        const step = Number(stepStr);
        const algo = (cells[colIndex.algo] || 'QS').trim();
        const opt = (cells[colIndex.opt] || 'GA').trim();
        const fitness_ms = Number(cells[colIndex.fitness_ms] || 0);
        const comparisons = Number(cells[colIndex.comparisons] || 0);
        const swaps = Number(cells[colIndex.swaps] || 0);
        const n = Number(cells[colIndex.n] || 0);
        const pivot = cells[colIndex.pivot] || '';
        const scheme = cells[colIndex.scheme] || '';
        const cutoff = cells[colIndex.cutoff] || '';
        const depth = cells[colIndex.depth] || '';
        const tail = cells[colIndex.tail] || '';
        const run_threshold = cells[colIndex.run_threshold] || '';
        const iterative = cells[colIndex.iterative] || '';
        const reuse_buffer = cells[colIndex.reuse_buffer] || '';
        const ga_idx = colIndex.ga_population_index != null ? cells[colIndex.ga_population_index] : (colIndex.pop_idx != null ? cells[colIndex.pop_idx] : '');
        const sa_temp = colIndex.sa_temperature != null ? cells[colIndex.sa_temperature] : (colIndex.temp != null ? cells[colIndex.temp] : '');

        const p = {
          step, algo, opt, fitness_ms, comparisons, swaps, n,
          dna: {
            pivot, scheme, cutoff, depth, tail, run_threshold, iterative, reuse_buffer
          },
          ga_idx, sa_temp
        };
        points.push(p);
        minComp = Math.min(minComp, comparisons); maxComp = Math.max(maxComp, comparisons);
        minSwp = Math.min(minSwp, swaps); maxSwp = Math.max(maxSwp, swaps);
        minFit = Math.min(minFit, fitness_ms); maxFit = Math.max(maxFit, fitness_ms);
        stepsMin = Math.min(stepsMin, step); stepsMax = Math.max(stepsMax, step);
      }else{
        // simple schema fallback
        const gen = Number(cells[0] || 0);
        const fitness_ms = Number(cells[5] || 0);
        const comparisons = Number(cells[6] || 0);
        const swaps = Number(cells[7] || 0);
        const p = {
          step: gen, algo: 'QS', opt: 'GA', fitness_ms, comparisons, swaps, n: 100000,
          dna: {
            pivot: cells[2] || '', scheme: cells[3] || '', cutoff: cells[4] || '',
            depth: '', tail: '', run_threshold: '', iterative: '', reuse_buffer: ''
          },
          ga_idx: cells[1] || '', sa_temp: ''
        };
        points.push(p);
        minComp = Math.min(minComp, comparisons); maxComp = Math.max(maxComp, comparisons);
        minSwp = Math.min(minSwp, swaps); maxSwp = Math.max(maxSwp, swaps);
        minFit = Math.min(minFit, fitness_ms); maxFit = Math.max(maxFit, fitness_ms);
        stepsMin = Math.min(stepsMin, gen); stepsMax = Math.max(stepsMax, gen);
      }
    }
    // normalize
    const compRange = Math.max(1, maxComp - minComp);
    const swpRange = Math.max(1, maxSwp - minSwp);
    const fitRange = Math.max(1e-9, maxFit - minFit);
    for(const p of points){
      p.normComparisons = (p.comparisons - minComp) / compRange;
      p.normSwaps = (p.swaps - minSwp) / swpRange;
      p.normFitness = (p.fitness_ms - minFit) / fitRange;
    }
    isHeavyDataset = points.length > 1000000;
    computeFitnessQuantiles();
    currentStep = stepsMin;
    isPlaying = false;
    playPauseBtn.textContent = 'Play';
    lastRenderStep = -1;
    cachedFiltered = null;
    requestAnimationFrame(tick);
  }

  function computeFitnessQuantiles(){
    fitnessQuantilesByStep.clear();
    const map = new Map();
    for(const p of points){
      if(!map.has(p.step)) map.set(p.step, []);
      map.get(p.step).push(p.fitness_ms);
    }
    for(const [step, arr] of map.entries()){
      arr.sort((a,b)=>a-b);
      const n = arr.length;
      if(!n) continue;
      const q = (r) => arr[Math.min(n-1, Math.max(0, Math.floor(r*(n-1))))];
      fitnessQuantilesByStep.set(step, { q25: q(0.25), q75: q(0.75) });
    }
  }
  async function loadCSVFromUrl(url){
    try{
      const res = await fetch(url);
      if(!res.ok) throw new Error(`HTTP ${res.status}`);
      const text = await res.text();
      parseCSV(text);
    }catch(e){
      alert('Failed to load CSV from URL: ' + e.message);
    }
  }
  function projectPoint(p, w, h){
    const k = 1.4;
    const scale = 1 / (1 + k * p.normFitness);
    const xCentered = (p.normComparisons - 0.5) * w;
    const yCentered = (p.normSwaps - 0.5) * h;
    return {
      x: w / 2 + xCentered * scale,
      y: h / 2 + yCentered * scale,
      r: Math.max(1.5, 3.2 * scale),
      scale
    };
  }
  function particleColor(p){
    if(colorMode && colorMode.value === 'algorithm'){
      return p.algo === 'QS' ? '#3fa7ff' : '#ff8e3f';
    }
    const q = fitnessQuantilesByStep.get(p.step);
    if(!q) return '#7bc96f';
    if(p.fitness_ms <= q.q25) return '#28a745';
    if(p.fitness_ms <= q.q75) return '#ffeb3b';
    return '#f44336';
  }
  function estimateSpace(p){
    if(p.algo === 'QS'){
      const d = p.dna || {};
      const depthCap = Number(d.depth || 0);
      const n = Number(p.n || 0) || 100000;
      const logn = Math.max(1, Math.floor(Math.log2(n)));
      const typical = `O(log n) stack (~${Math.min(depthCap || logn, logn)})`;
      const worst = (d.pivot === 'First' || d.pivot === 'Last') ? 'worst O(n) if adversarial' : 'balanced pivots typical';
      const tail = d.tail ? ', tail-elim' : '';
      return `${typical}${tail}; ${worst}`;
    }else{
      const d = p.dna || {};
      const buf = 'O(n) buffer';
      const iter = (d.iterative === '1' || d.iterative === 'true' || d.iterative === true) ? ', iterative' : '';
      const reuse = (d.reuse_buffer === '1' || d.reuse_buffer === 'true' || d.reuse_buffer === true) ? ', reuse buffer' : '';
      return `${buf}${iter}${reuse}`;
    }
  }
  function updateLeaderboard(maxStep){
    const stepThreshold = maxStep !== undefined ? maxStep : stepsMax;
    
    const optFilterVal = optFilter.value;
    const algoFilterVal = algoFilter.value;
    const shouldIncludeGA = (optFilterVal === 'all' || optFilterVal === 'GA');
    const shouldIncludeSA = (optFilterVal === 'all' || optFilterVal === 'SA');
    const shouldIncludeQS = (algoFilterVal === 'all' || algoFilterVal === 'QS');
    const shouldIncludeMS = (algoFilterVal === 'all' || algoFilterVal === 'MS');
    
    let bestQS = null, bestMS = null, bestGA = null, bestSA = null;
    let hasGA = false, hasSA = false, hasQS = false, hasMS = false;
    
    for(const p of points){
      if(p.step > stepThreshold + 0.5) continue;
      
      if(p.opt === 'GA' && !shouldIncludeGA) continue;
      if(p.opt === 'SA' && !shouldIncludeSA) continue;
      if(p.algo === 'QS' && !shouldIncludeQS) continue;
      if(p.algo === 'MS' && !shouldIncludeMS) continue;
      
      if(p.algo === 'QS' && shouldIncludeQS){
        hasQS = true;
        if(!bestQS || p.fitness_ms < bestQS.fitness_ms) bestQS = p;
      }
      if(p.algo === 'MS' && shouldIncludeMS){
        hasMS = true;
        if(!bestMS || p.fitness_ms < bestMS.fitness_ms) bestMS = p;
      }
      if(p.opt === 'GA' && shouldIncludeGA){
        hasGA = true;
        if(!bestGA || p.fitness_ms < bestGA.fitness_ms) bestGA = p;
      }
      if(p.opt === 'SA' && shouldIncludeSA){
        hasSA = true;
        if(!bestSA || p.fitness_ms < bestSA.fitness_ms) bestSA = p;
      }
    }   
    lbQS && (lbQS.textContent = (hasQS && shouldIncludeQS && bestQS) ? `${bestQS.fitness_ms.toFixed(3)} ms` : '—');
    lbMS && (lbMS.textContent = (hasMS && shouldIncludeMS && bestMS) ? `${bestMS.fitness_ms.toFixed(3)} ms` : '—');
    lbGA && (lbGA.textContent = (hasGA && shouldIncludeGA && bestGA) ? `${bestGA.fitness_ms.toFixed(3)} ms` : '—');
    lbSA && (lbSA.textContent = (hasSA && shouldIncludeSA && bestSA) ? `${bestSA.fitness_ms.toFixed(3)} ms` : '—');
  }

  function tick(ts){
    const dt = lastTs ? Math.min(100, ts - lastTs) : 0;
    lastTs = ts;
    if(isPlaying){
      currentStep += (dt / 1000) * speed;
      if(currentStep > stepsMax + 0.999) currentStep = stepsMax + 0.999;
    }
    
    render();
    requestAnimationFrame(tick);
  }
  function render(){
    const w = canvas.clientWidth || canvas.width;
    const h = canvas.clientHeight || canvas.height;
    ctx.clearRect(0, 0, w, h);
    
    if(!render.bgGradient || render.bgW !== w || render.bgH !== h){
      const g = ctx.createRadialGradient(w*0.5, h*0.4, Math.min(w,h)*0.05, w*0.5, h*0.6, Math.max(w,h)*0.8);
      g.addColorStop(0, '#0b0f14');
      g.addColorStop(1, '#0a0f16');
      render.bgGradient = g;
      render.bgW = w;
      render.bgH = h;
    }
    ctx.fillStyle = render.bgGradient;
    ctx.fillRect(0, 0, w, h);

    if(points.length === 0) return;

    const maxStepToRender = Math.floor(currentStep);
    const algoVal = algoFilter.value;
    const optVal = optFilter.value;
    
    const cacheKey = `${maxStepToRender}-${algoVal}-${optVal}`;
    if(cachedFiltered && cachedFiltered.cacheKey === cacheKey && maxStepToRender === lastRenderStep){
      var filtered = cachedFiltered.data;
    }else{
      if(isHeavyDataset){
        filtered = points.filter(p => 
          p.step === maxStepToRender && 
          (algoVal === 'all' || p.algo === algoVal) && 
          (optVal === 'all' || p.opt === optVal)
        );
      }else{
        filtered = points.filter(p => 
          p.step <= maxStepToRender && 
          (algoVal === 'all' || p.algo === algoVal) && 
          (optVal === 'all' || p.opt === optVal)
        );
      }
      cachedFiltered = { cacheKey, data: filtered };
      lastRenderStep = maxStepToRender;
    }

    if(revealMode.value === 'progressive'){
      const stepsSpan = Math.max(1, stepsMax - stepsMin + 1);
      const t = (maxStepToRender - stepsMin) / stepsSpan;
      const target = Math.max(2, Math.floor(2 + t * 100000));
      if(filtered.length > target) filtered = filtered.slice(0, target);
    }else if(revealMode.value === 'top1k'){
      if(filtered.length > 1000){
        filtered = filtered.slice().sort((a, b) => a.fitness_ms - b.fitness_ms).slice(0, 1000);
      }
    }
    let best = null;
    if(filtered.length > 0){
      for(const p of filtered){
        if(!best || p.fitness_ms < best.fitness_ms) best = p;
      }
    }
    if(filtered.length > 100){
      filtered.sort((a, b) => a.normFitness - b.normFitness);
    }
    ctx.save();
    ctx.imageSmoothingEnabled = false;

    const baseDrawCap = Math.min(50000, Math.floor((w * h) / 100));
    const DRAW_CAP = Math.min(baseDrawCap, filtered.length);
    
    const seen = new Set();
    const hash = (x, y) => ((x|0) << 16) ^ (y|0);
    
    let currentColor = null;
    let currentShadow = null;
    let draws = 0;
    
    const colorCache = new Map();
    const getCachedColor = (p) => {
      const key = `${p.algo}-${p.opt}-${p.step}-${p.fitness_ms}`;
      if(!colorCache.has(key)){
        colorCache.set(key, particleColor(p));
      }
      return colorCache.get(key);
    };
    for(const p of filtered){
      if(draws >= DRAW_CAP) break;
      
      const pr = projectPoint(p, w, h);
      
      if(pr.x + pr.r < 0 || pr.x - pr.r > w || pr.y + pr.r < 0 || pr.y - pr.r > h){
        continue;
      }
      
      const gx = (pr.x * 0.75) | 0;
      const gy = (pr.y * 0.75) | 0;
      const key = hash(gx, gy);
      if(seen.has(key)) continue;
      seen.add(key);

      const color = getCachedColor(p);
      const shadowBlur = Math.max(0, 8 * pr.scale);
      
      if(currentColor !== color){
        ctx.fillStyle = color;
        currentColor = color;
      }
      if(currentShadow !== shadowBlur){
        ctx.shadowBlur = shadowBlur;
        ctx.shadowColor = color;
        currentShadow = shadowBlur;
      }

      ctx.beginPath();
      ctx.arc(pr.x, pr.y, pr.r, 0, Math.PI * 2);
      ctx.fill();

      draws++;
    }
    
    if(colorCache.size > 10000){
      colorCache.clear();
    }
    
    ctx.restore();
    const totalSteps = stepsMax >= stepsMin ? (stepsMax - stepsMin + 1) : 1;
    const currentStepNum = Math.floor(maxStepToRender) - stepsMin + 1;
    stepStat.textContent = `${Math.max(1, currentStepNum)} / ${totalSteps}`;
    countStat.textContent = filtered.length.toLocaleString();
    bestFitnessStat.textContent = best ? `${best.fitness_ms.toFixed(3)} ms` : '—';
    if(best){
      bestDNA.textContent = formatDNA(best);
      spaceStat && (spaceStat.textContent = estimateSpace(best));
    }
    
    const prevStep = render.leaderboardCache;
    const currentFilters = `${maxStepToRender}-${optFilter.value}-${algoFilter.value}`;
    const prevFilters = render.leaderboardFilters;
    if(maxStepToRender !== prevStep || currentFilters !== prevFilters){
      updateLeaderboard(maxStepToRender);
      render.leaderboardCache = maxStepToRender;
      render.leaderboardFilters = currentFilters;
    }
  }
  render.leaderboardCache = -1;
  render.leaderboardFilters = '';

  function formatDNA(p){
    const d = p.dna || {};
    return [
      `Algo: ${p.algo} | Opt: ${p.opt}`,
      `Fitness: ${p.fitness_ms.toFixed(3)} ms  |  n=${p.n}`,
      `Comparisons: ${p.comparisons.toLocaleString()} | Swaps: ${p.swaps.toLocaleString()}`,
      p.algo === 'QS'
        ? `Pivot=${d.pivot}  Scheme=${d.scheme}  Cutoff=${d.cutoff}  Depth=${d.depth}  Tail=${d.tail}`
        : `RunThresh=${d.run_threshold}  Iterative=${d.iterative}  ReuseBuf=${d.reuse_buffer}`
    ].join('\n');
  }
  playPauseBtn.addEventListener('click', () => {
    isPlaying = !isPlaying;
    playPauseBtn.textContent = isPlaying ? 'Pause' : 'Play';
  });
  resetBtn.addEventListener('click', () => {
    currentStep = stepsMin;
    isPlaying = false;
    playPauseBtn.textContent = 'Play';
    render();
  });
  prevBtn.addEventListener('click', () => {
    currentStep = Math.max(stepsMin, Math.floor(currentStep) - 1);
    isPlaying = false;
    playPauseBtn.textContent = 'Play';
    render();
  });
  nextBtn.addEventListener('click', () => {
    currentStep = Math.min(stepsMax + 0.999, Math.floor(currentStep) + 1);
    isPlaying = false;
    playPauseBtn.textContent = 'Play';
    render();
  });
  speedSlider.addEventListener('input', () => {
    speed = Number(speedSlider.value || '1');
  });
  const clearCacheAndRender = () => {
    cachedFiltered = null;
    lastRenderStep = -1;
    render();
  };
  optFilter.addEventListener('change', clearCacheAndRender);
  algoFilter.addEventListener('change', clearCacheAndRender);
  revealMode.addEventListener('change', clearCacheAndRender);
  colorMode && colorMode.addEventListener('change', clearCacheAndRender);

  canvas.addEventListener('click', (e) => {
    if(points.length === 0) return;
    const rect = canvas.getBoundingClientRect();
    const mx = e.clientX - rect.left;
    const my = e.clientY - rect.top;
    const maxStepToRender = Math.floor(currentStep);
    let filtered = points.filter(p => p.step <= maxStepToRender);
    const algoVal = algoFilter.value;
    const optVal = optFilter.value;
    if(algoVal !== 'all') filtered = filtered.filter(p => p.algo === algoVal);
    if(optVal !== 'all') filtered = filtered.filter(p => p.opt === optVal);

    let closest = null, bestDist = Infinity;
    for(const p of filtered){
      const pr = projectPoint(p, canvas.clientWidth, canvas.clientHeight);
      const dx = pr.x - mx;
      const dy = pr.y - my;
      const d2 = dx*dx + dy*dy;
      if(d2 < bestDist && d2 < 15*15){
        bestDist = d2;
        closest = p;
      }
    }
    if(closest) showModal(closest);
  });
  function showModal(p){
    modalBody.innerHTML = `
      <div><strong>${p.algo}</strong> via <strong>${p.opt}</strong></div>
      <div>Step: ${p.step}</div>
      <div>Fitness: ${p.fitness_ms.toFixed(3)} ms</div>
      <div>Comparisons: ${p.comparisons.toLocaleString()}</div>
      <div>Swaps: ${p.swaps.toLocaleString()}</div>
      <hr>
      ${p.algo === 'QS' ? `
        <div>Pivot: ${p.dna.pivot}</div>
        <div>Partition: ${p.dna.scheme}</div>
        <div>Cutoff: ${p.dna.cutoff}</div>
        <div>Depth cap: ${p.dna.depth}</div>
        <div>Tail recursion: ${p.dna.tail}</div>
      ` : `
        <div>Run threshold: ${p.dna.run_threshold}</div>
        <div>Iterative: ${p.dna.iterative}</div>
        <div>Reuse buffer: ${p.dna.reuse_buffer}</div>
      `}
      ${p.ga_idx ? `<div>GA index: ${p.ga_idx}</div>` : ''}
      ${p.sa_temp ? `<div>SA temp: ${p.sa_temp}</div>` : ''}
      <div style="margin-top:6px; opacity:0.9;">Space: ${estimateSpace(p)}</div>
    `;
    modal.classList.remove('hidden');
  }
  modalClose.addEventListener('click', () => modal.classList.add('hidden'));
  modal.addEventListener('click', (e) => { if(e.target === modal) modal.classList.add('hidden'); });
  (function initQueryLoad(){
    const params = new URLSearchParams(location.search);
    const csvParam = params.get('csv');
    if(csvParam){
      csvUrlInput.value = csvParam;
      localStorage.setItem('viz_csv_url', csvParam);
      loadCSVFromUrl(csvParam);
    }
  })();
  window.addEventListener('paste', (e) => {
    const text = (e.clipboardData || window.clipboardData).getData('text');
    if(text && /\.csv(\?|$)/i.test(text)){
      csvUrlInput.value = text.trim();
      localStorage.setItem('viz_csv_url', csvUrlInput.value);
      loadCSVFromUrl(csvUrlInput.value);
    }
  });
})();

