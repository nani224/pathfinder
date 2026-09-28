import subprocess
import sys

def run_test():
    cmd = [r"bin\pathfinder.exe", r"data\graph.txt"]
    print("[*] Running PathFinder Automated Verification Suite...")
    
    # Test script commands passed via stdin:
    # 1. 'list' -> check node listing
    # 2. 'France' 'India' -> verify shortest path
    # 3. 'next' -> verify alternative route
    # 4. 'new' -> new search
    # 5. 'Spain' 'Portugal' -> direct neighbor search
    # 6. 'new'
    # 7. 'NonExistent' 'India' -> verify error handling
    # 8. 'exit' -> exit cleanly
    inputs = (
        "list\n"
        "France India\n"
        "next\n"
        "new\n"
        "Spain Portugal\n"
        "new\n"
        "FakeCity India\n"
        "exit\n"
    )
    
    proc = subprocess.Popen(
        cmd,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    stdout, stderr = proc.communicate(input=inputs, timeout=5)
    
    print("\n--- Process Output Summary ---")
    print(stdout[:1000])
    print("...\n------------------------------")
    
    # Assertions
    assert "Graph loaded successfully with 22 nodes" in stdout, "Node count mismatch"
    print("[+] Graph loading (22 nodes): PASS")
    
    assert "Available Nodes (22 total):" in stdout, "Node listing failed"
    print("[+] Node listing (list command): PASS")
    
    assert "France -> Belgium -> Netherlands -> Germany -> India" in stdout, f"Route mismatch: {stdout}"
    assert "2823.00 km" in stdout, "Shortest path distance mismatch"
    print("[+] Dijkstra optimal path France -> India (2823.00 km): PASS")
    
    assert "Alternative Route #2" in stdout, "K-alternative path calculation failed"
    print("[+] K-alternative next-best path enumeration: PASS")
    
    assert "Spain -> Portugal" in stdout and "620.00 km" in stdout, "Direct edge search failed"
    print("[+] Spain -> Portugal shortest path (620.00 km): PASS")
    
    assert "No path exists between 'FakeCity' and 'India'" in stdout, "Missing error handling on invalid node"
    print("[+] Invalid node error handling: PASS")
    
    print("\n>>> ALL PATHFINDER ALGORITHM TESTS PASSED 100%! <<<")
    return True

if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
