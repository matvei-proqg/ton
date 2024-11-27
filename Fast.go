package main

import (
    "crypto/sha256"
    "encoding/hex"
    "fmt"
    "sync"
    "time"
)

// Transaction представляет собой транзакцию
type Transaction struct {
    Sender    string
    Recipient string
    Amount    float64
}

// Block представляет собой блок в цепочке
type Block struct {
    Index        int
    Timestamp    string
    Transactions []Transaction
    PreviousHash string
    Hash         string
}

// Blockchain представляет собой цепочку блоков
type Blockchain struct {
    blocks []*Block
    mu     sync.Mutex
}

// NewBlock создает новый блок
func NewBlock(index int, transactions []Transaction, previousHash string) *Block {
    block := &Block{
        Index:        index,
        Timestamp:    time.Now().String(),
        Transactions: transactions,
        PreviousHash: previousHash,
    }
    block.Hash = block.calculateHash()
    return block
}

// calculateHash вычисляет хэш блока
func (b *Block) calculateHash() string {
    record := fmt.Sprintf("%d%s%v%s", b.Index, b.Timestamp, b.Transactions, b.PreviousHash)
    h := sha256.New()
    h.Write([]byte(record))
    return hex.EncodeToString(h.Sum(nil))
}

// NewBlockchain создает новый блокчейн
func NewBlockchain() *Blockchain {
    return &Blockchain{
        blocks: []*Block{NewBlock(0, []Transaction{}, "")}, // Генезис блок
    }
}

// AddBlock добавляет новый блок в цепочку
func (bc *Blockchain) AddBlock(transactions []Transaction) {
    bc.mu.Lock()
    defer bc.mu.Unlock()
    lastBlock := bc.blocks[len(bc.blocks)-1]
    newBlock := NewBlock(lastBlock.Index+1, transactions, lastBlock.Hash)
    bc.blocks = append(bc.blocks, newBlock)
}

// PrintBlocks выводит информацию о блоках
func (bc *Blockchain) PrintBlocks() {
    for _, block := range bc.blocks {
        fmt.Printf("Index: %d\n", block.Index)
        fmt.Printf("Timestamp: %s\n", block.Timestamp)
        fmt.Printf("Transactions: %v\n", block.Transactions)
        fmt.Printf("Previous Hash: %s\n", block.PreviousHash)
        fmt.Printf("Hash: %s\n", block.Hash)
        fmt.Println()
    }
}

func main() {
    blockchain := NewBlockchain()

    // Пример добавления транзакций
    transactions1 := []Transaction{
        {"Alice", "Bob", 10.0},
        {"Bob", "Charlie", 5.0},
    }
    blockchain.AddBlock(transactions1)

    transactions2 := []Transaction{
        {"Charlie", "David", 2.0},
    }
    blockchain.AddBlock(transactions2)

    // Печать блоков
    blockchain.PrintBlocks()
}
